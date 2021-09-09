#include <dark/files.h>

#include <skyrim/skeleton.h>
#include <skyrim/record.h>

#include <renderer/group.h>
#include <renderer/renderer.h>

#include <glm/gtc/quaternion.hpp>
#include <algorithm>

using namespace dark;

namespace skyrim
{
	static void ni_node_callback(Rd *, NiNode *);

	Keyframes *skyrim_get_keyframes(const char *path)
	{
		return load_keyframes_from_disk(path);
	}

	Skeleton::Skeleton()
	{
		baseBone = new Bone();
		bones[-1] = baseBone;
		lastBone = baseBone;
		model = nullptr;
		animation = nullptr;
	}

	Skeleton::Skeleton(Record race) : Skeleton()
	{
		model = load_model(load_res(race.data<char *>("ANAM")));
		Construct();
	}

	void Skeleton::Construct()
	{
		Rd *rd = calloc_nifprd();
		rd->nif = model;
		rd->data = this;
		//rd->other = other;
		rd->ni_node_callback = ni_node_callback;
		nif_rd(rd);
		free_nifprd(&rd);
		baseBone->Update();
	}

	Bone *Skeleton::MakeBoneHere(Rd *rd, int name)
	{
		Bone *bone = new Bone();
		bones[rd->current] = bone;
		bones[rd->parent]->Add(bone);
		bonesNamed[nif_get_string(rd->nif, name)] = bone;
		lastBone = bone;
		return bone;
	}

	void matrix_from_common(Bone *bone, ni_common_layout_t *common)
	{
		bone->matrix = translate(bone->matrix, cast_vec3(&common->A->translation));
		bone->matrix *= inverse(mat4(cast_mat3(&common->A->rotation)));
		bone->matrix = scale(bone->matrix, vec3(common->A->scale));
		bone->Update();
		bone->rest = bone->matrixWorld;
	}

	void ni_node_callback(Rd *rd, NiNode *block)
	{
		Skeleton *skeleton = (Skeleton *)rd->data;
		Bone *bone = skeleton->MakeBoneHere(rd, block->common->F->name);
		matrix_from_common(bone, block->common);
	}

	void Skeleton::Step()
	{
		if (animation)
			animation->Step();
	}

	Keyframes::Keyframes(Nif *model) : model(model)
	{
		loop = true;
		controllerSequence = nullptr;
		if (model == nullptr)
			return;
		assertc(strcmp(model->hdr->block_types[0], NiControllerSequenceS) == 0);
		controllerSequence = (NiControllerSequence *)model->blocks[0];
	}

	Animation::Animation(Keyframes *keyframes) : keyframes(keyframes)
	{
		skeleton = nullptr;
		time = 0;
		play = true;
		if (keyframes == nullptr)
			play = false;
	}

	void Animation::Step()
	{
		if (!play)
			return;
		time += delta;
		if (time >= keyframes->controllerSequence->C->stop_time)
			time -= keyframes->controllerSequence->C->stop_time;
		SimpleNonInterpolated();
		skeleton->baseBone->Update();
	}

	void Animation::SimpleNonInterpolated()
	{
		Nif *model = keyframes->model;
		struct controlled_block_t *cbp;
		for (unsigned int i = 0; i < keyframes->controllerSequence->A->num_controlled_blocks; i++)
		{
			// Match node_name to a skeleton bone
			cbp = &keyframes->controllerSequence->controlled_blocks[i];
			char *name = nif_get_string(model, cbp->node_name);
			auto has = skeleton->bonesNamed.find(name);
			if (has == skeleton->bonesNamed.end())
			{
				//printf("cant find bone %s\n", name);
				// cant find shield, weapon, quiver
				continue;
			}

			Bone *bone = has->second;
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
