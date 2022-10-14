#include <miryks/skeleton.h>

#include <opengl/scene_graph.hpp>
#include <opengl/renderer.h>

#include <files.h>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace miryks
{
	static void ni_node_callback(RD, NiNode *);

	keyframes *get_keyframes(const char *path)
	{
		return load_keyframes_from_disk(path);
	}

	skeleton::skeleton()
	{
		anim = nullptr;
		mixer = nullptr;
		model = nullptr;
		bones[-1] = new bone();
	}

	skeleton::skeleton(const char *race) : skeleton()
	{
		printf("skeleton anam %s\n", race);
		model = get_ni(get_res(race));
		Construct();
	}

	skeleton::~skeleton()
	{
	}

	void skeleton::Construct()
	{
		rundown *rd = calloc_rd();
		rd->ni = model;
		rd->data = this;
		rd->ni_node_callback = ni_node_callback;
		nif_rd(rd);
		free_rd(&rd);
		assertc(root);
		root->UpdateSideways();
	}

	GroupDrawer *skeleton::ShowBones()
	{
		return new GroupDrawer(this->baseBone, mat4(1.0));
	}

	void matrix_from_common(bone *bone, ni_common_layout_t *common)
	{
		bone->matrix = translate(bone->matrix, reinterpret_vec3(&common->A->translation));
		bone->matrix *= inverse(mat4(reinterpret_mat3(&common->A->rotation)));
		bone->matrix = scale(bone->matrix, vec3(common->A->scale));
		bone->UpdateSideways();
		bone->rest = bone->matrixWorld;
		bone->Cur();
	}

	bone *skeleton::MakeBoneHere(RD rd, NiNode *block)
	{
		// printf("bone name is %s\n", bone->name);
		bone *bon = new bone;
		bone *bone = bon;
		bone->block = block;
		bone->name = nif_get_string(rd->ni, block->common->F->name);
		bones[rd->current] = bone;
		bones[rd->parent]->Add(bone);
		bonesNamed[bone->name] = bone;
		matrix_from_common(bone, block->common);
		if (strstr(bone->name, "[Root]"))
		{
			root = bone;
		}
		return bon;
	}

	void ni_node_callback(RD rd, NiNode *block)
	{
		skeleton *skel = (skeleton *)rd->data;
		bone *bone = skel->MakeBoneHere(rd, block);
	}

	void skeleton::Step()
	{
		// if (mixer)
		//	mixer->Step();
		if (anim)
			anim->Step();
	}

	/*skeleton *skeleton::Copy()
	{
		skeleton *copy = new skeleton;
		return copy;
	}*/

	keyframes::keyframes(nif *ni) : ni(ni)
	{
		loop = true;
		repeats = true;
		controllerSequence = nullptr;
		if (ni == nullptr)
			return;
		assertc(strcmp(ni->hdr->block_types[0], NiControllerSequenceS) == 0);
		controllerSequence = (NiControllerSequence *)ni->blocks[0];
	}

	animation::~animation()
	{
	}

	animation::animation(keyframes *keyf) : keyf(keyf)
	{
		skel = nullptr;
		next = nullptr;
		time = 0;
		ratio = 0;
		first = true;
		play = true;
		loop = false;
		if (keyf == nullptr)
			play = false;
	}

	void animation::Step()
	{
		if (!play)
			return;

		if (renderSettings.slowAnimation)
			time += delta / 4;
		else
			time += delta;

		if (time >= keyf->controllerSequence->C->stop_time)
		{
			if (loop)
				time -= keyf->controllerSequence->C->stop_time;
			else if (next)
			{
				skel->anim = next;
				skel->SetFreeze();
				delete this;
				return;
			}
		}

		if (first)
			SimpleInterpolated();
		first = false;
		SimpleInterpolated();
		skel->root->UpdateSideways();
	}

	void skeleton::SetFreeze()
	{
		for (auto pair : bones)
		{
			bone *bone = pair.second;
			bone->Cur();
			bone->frozenQ = bone->curQ;
			bone->frozenV = bone->curV;
		}
	}

	void animation::SimpleInterpolated()
	{
		ratio += delta * 3; // one third of a second
		ratio = ratio <= 0 ? 0 : ratio >= 1 ? 1 : ratio;
		int cache = 0;
		nif *model = keyf->ni;
		// controlled block pointer
		struct controlled_block_t *cbp;
		for (unsigned int i = 0; i < keyf->controllerSequence->A->num_controlled_blocks; i++)
		{
			cbp = &keyf->controllerSequence->controlled_blocks[i];
			if (first)
			{
				char *name = nif_get_string(model, cbp->node_name);
				// printf("name %s\n", name);
				auto has = skel->bonesNamed.find(name);
				if (has == skel->bonesNamed.end())
				{
					// printf("cant find bone %s\n", name);
					//  cant find shield, weapon, quiver
					boneCache.push_back(nullptr);
					continue;
				}
				bone *bone = has->second;
				boneCache.push_back(bone);
			}
			else
			{
				const bool interpolate = true;
				bone *bone = boneCache[cache++];
				if (bone == nullptr)
					continue;
				auto tip = (NiTransformInterpolator *)nif_get_block(model, cbp->interpolator);
				auto tdp = (NiTransformData *)nif_get_block(model, tip->B->data);
				if (tip == NULL || tdp == NULL)
					continue;
				vec4 ro = reinterpret_vec4(&tip->transform->rotation);
				quat qro = quat(0, 0, 0, 0); // uninitializing this meant hard to debug invisibility bugs
				int num = tdp->A->num_rotation_keys;
				if (num)
				{
					for (int i = num - 1; i >= 0; i--)
					{
						auto key = &tdp->quaternion_keys[i];
						// printf("qk %i time %f\n", i, key->time);
						if (key->time <= time)
						{
							ro = reinterpret_vec4(&key->value);
							qro = quat(ro[0], ro[1], ro[2], ro[3]);

							if (interpolate)
							{
								int j = i + 1;
								if (j >= num)
									break;
								auto key2 = &tdp->quaternion_keys[j];
								vec4 rn = reinterpret_vec4(&key2->value);

								float span = key2->time - key->time;
								float ratio = (time - key->time) / span;
								quat quat1 = quat(ro[0], ro[1], ro[2], ro[3]);
								quat quat2 = quat(rn[0], rn[1], rn[2], rn[3]);

								// printf("r ratio %.2f\n", ratio);
								// assertc(ratio >= 0 || ratio <= 1);

								qro = glm::slerp(quat1, quat2, ratio);
							}
							break;
						}
					}
				}
				// else if (num == 1)
				// 	ro = *cast_vec_4((float *)(&tdp->quaternion_keys[0].value));

				vec3 tr = reinterpret_vec3(&tip->transform->translation);
				num = tdp->translations->num_keys;
				if (num)
				{
					for (int i = num - 1; i >= 0; i--)
					{
						auto key = &tdp->translation_keys[i];
						if (key->time <= time)
						{
							// printf("tr time %f\n", key->time);
							tr = reinterpret_vec3(&key->value);

							if (interpolate)
							{
								int j = i + 1;
								if (j >= num)
									break;
								auto key2 = &tdp->translation_keys[j];
								vec3 tr2 = reinterpret_vec3(&key2->value);

								float span = key2->time - key->time;
								float ratio = (time - key->time) / span;

								// assertc(ratio >= 0 || ratio <= 1);

								// ratio = (ratio > 1)? 1 : (ratio < 0)? 0 : ratio;
								// printf("t ratio %.2f time %.2f\n ", ratio, key->time);
								tr = glm::mix(tr, tr2, ratio);
							}
							break;
						}
					}
				}

				quat q = slerp(bone->frozenQ, qro, ratio);
				vec4 v = mix(bone->frozenV, vec4(tr, 1), ratio);
				mat4 matrix = mat4_cast(q);
				matrix[3] = v;
				bone->matrix = matrix;
			}
		}
	}

} // namespace dark
