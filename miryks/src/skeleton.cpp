#include <miryks/skeleton.h>

#include <opengl/group.h>
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
		model = nullptr;
		bones[-1] = new bone();
	}

	skeleton::skeleton(char *race) : skeleton()
	{
		printf("skeleton anam %s\n", race);
		model = get_ni(get_res(race));
		Construct();
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
		root->Update();
	}

	DrawGroup *skeleton::ShowBones()
	{
		return new DrawGroup(this->baseBone, mat4(1.0));
	}

	void matrix_from_common(bone *bone, ni_common_layout_t *common)
	{
		bone->matrix = translate(bone->matrix, cast_vec3(&common->A->translation));
		bone->matrix *= inverse(mat4(cast_mat3(&common->A->rotation)));
		bone->matrix = scale(bone->matrix, vec3(common->A->scale));
		bone->Update();
		bone->rest = bone->matrixWorld;
	}

	bone *skeleton::MakeBoneHere(RD rd, NiNode *block)
	{
		//printf("bone name is %s\n", bone->name);
		auto *bon = new bone();
		bon->block = block;
		bon->name = nif_get_string(rd->ni, block->common->F->name);
		bones[rd->current] = bon;
		bones[rd->parent]->Add(bon);
		bonesNamed[bon->name] = bon;
		matrix_from_common(bon, block->common);
		if (strstr(bon->name, "[Root]")) {
			root = bon;
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
		if (anim)
			anim->Step();
	}

	keyframes::keyframes(nif *ni) : ni(ni)
	{
		loop = true;
		controllerSequence = nullptr;
		if (ni == nullptr)
			return;
		assertc(strcmp(ni->hdr->block_types[0], NiControllerSequenceS) == 0);
		controllerSequence = (NiControllerSequence *)ni->blocks[0];
	}

	animation::animation(keyframes *keyf) : keyf(keyf)
	{
		skel = nullptr;
		time = 0;
		play = true;
		if (keyf == nullptr)
			play = false;
	}

	void animation::Step()
	{
		if (!play)
			return;
		time += delta;
		if (time >= keyf->controllerSequence->C->stop_time)
			time -= keyf->controllerSequence->C->stop_time;
		SimpleNonInterpolated();
		skel->root->Update();
	}

	void animation::SimpleNonInterpolated()
	{
		nif *model = keyf->ni;
		struct controlled_block_t *cbp;
		for (unsigned int i = 0; i < keyf->controllerSequence->A->num_controlled_blocks; i++)
		{
			// Match node_name to a skeleton bone
			cbp = &keyf->controllerSequence->controlled_blocks[i];
			char *name = nif_get_string(model, cbp->node_name);
			auto has = skel->bonesNamed.find(name);
			if (has == skel->bonesNamed.end())
			{
				//printf("cant find bone %s\n", name);
				// cant find shield, weapon, quiver
				continue;
			}

			bone *bone = has->second;
			auto tip = (NiTransformInterpolator *)nif_get_block(model, cbp->interpolator);
			auto tdp = (NiTransformData *)nif_get_block(model, tip->B->data);
			if (tip == NULL || tdp == NULL)
				continue;
			vec4 ro = cast_vec4(&tip->transform->rotation);
			int num = tdp->A->num_rotation_keys;
			if (num)
			{
				for (int i = num - 1; i >= 0; i--)
				{
					auto key = &tdp->quaternion_keys[i];
					//printf("qk %i time %f\n", i, key->time);
					if (key->time <= time || num == 1)
					{
						ro = cast_vec4(&key->value);
						//printf("ro.x%f\n", ro.x);
						break;
					}
				}
			}
			// else if (num == 1)
			// 	ro = *cast_vec_4((float *)(&tdp->quaternion_keys[0].value));

			vec3 tr = cast_vec3(&tip->transform->translation);
			num = tdp->translations->num_keys;
			if (num)
			{
				for (int i = num - 1; i >= 0; i--)
				{
					auto key = &tdp->translation_keys[i];
					if (key->time <= time || num == 1)
					{
						//printf("tr time %f\n", key->time);
						tr = cast_vec3(&key->value);
						break;
					}
				}
			}
			// else if (num == 1)
			// 	tr = gloomVec3(tdp->translation_keys[0].value);

			quat qu = quat(ro[0], ro[1], ro[2], ro[3]);

			mat4 matrix = mat4_cast(qu);
			matrix[3] = vec4(tr, 1);

			bone->mod = matrix;
			bone->matrix = matrix;
			bone->diff = inverse(matrix) * bone->rest;
			bone->Update();
		}
	}

	
} // namespace dark
