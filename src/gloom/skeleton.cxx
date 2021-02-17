#include <algorithm>

#include <libs>
#include <Gloom/Skeleton.h>

#include <opengl/group.h>
#include <opengl/types.h>

#include <glm/gtc/quaternion.hpp>

namespace gloom
{
	static void ni_node_callback(NifpRd *, ni_node_pointer *);

	Skeleton::Skeleton()
	{
		baseBone = new Bone();
		bones[-1] = baseBone;
		lastBone = baseBone;
		nif = nullptr;
		animation = nullptr;
	}

	void Skeleton::Load(const char *ANAM)
	{
		// printf("skeleton load anam %s\n", ANAM);
		Rc *rc = loadRc("meshes\\", ANAM, 0x1);
		nif = loadNif(rc, true);
		// printf("num_blocks of skeleton %u\n", nif->hdr->num_blocks);
	}

	void Skeleton::Construct()
	{
		NifpRd *rd = malloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		//rd->other = other;
		rd->ni_node = ni_node_callback;
		nifp_rd(rd);
		free_nifprd(&rd);
		baseBone->group->Update();
	}

	Bone *Skeleton::Nested(NifpRd *rd, int name)
	{
		Bone *bone = new Bone();
		bones[rd->current] = bone;
		bones[rd->parent]->group->Add(bone->group);
		bones_named[nifp_get_string(rd->nif, name)] = bone;
		lastBone = bone;
		return bone;
	}

	void matrix_from_common(Bone *bone, ni_common_layout_pointer *common)
	{
		bone->group->matrix = translate(bone->group->matrix, Gloom_Vec_3(common->C->translation));
		bone->group->matrix *= inverse(mat4(Gloom_Mat_3(common->C->rotation)));
		bone->group->matrix = scale(bone->group->matrix, vec3(common->C->scale));
		bone->group->Update();
		bone->rest = bone->group->matrixWorld;
	}
	
	void ni_node_callback(NifpRd *rd, ni_node_pointer *block)
	{
		//printf("skelly ni node callback\n");
		Skeleton *skeleton = (Skeleton *)rd->data;
		Bone *bone = skeleton->Nested(rd, block->common->A->name);
		matrix_from_common(bone, block->common);
	}
	
	void Skeleton::Step()
	{
		if (animation)
			animation->Step();
	}

	// keyframes

	KeyFrames::KeyFrames(Nifp *nif) : model(nif)
	{
		cassert(strcmp(model->hdr->block_types[0], NI_CONTROLLER_SEQUENCE) == 0, "block 0 not a controller sequence");

		csp = (ni_controller_sequence_pointer *)model->blocks[0];
	}

	// Todo, This is a big dump from Messiah (before Dark (so long before Dark2))
	// Pasted this here to see if it would compile. It won't since it's templated >_< And I can't use it because it's dumb. 
	// animation

	template <typename T, typename Y>
	struct Lol
	{
		const T *one;
		const T *two;
		float ratio;
	};

	// If loop is true then also interpolate between
	// Last and first

	template <typename T, typename Y>
	Lol<T, Y> interpolate(Animation *animation, int num, const T *keys)
	{
		Nifp *nif = animation->keyframes->model;

		auto csp = animation->keyframes->csp;

		Lol<T, Y> s;
		s.one = nullptr;
		s.two = nullptr;
		s.ratio = 1;

		const auto loop = animation->keyframes->loop;

		int i, j;
		//num = keys.size();
		if (num > 1)
		{
			i = num - (loop ? 1 : 2);

			for (; i >= 0; i--)
			{
				//loop:
				const T &key_a = keys[i];
				j = i + 1;
				const bool got_two = j <= num - 1;

				if (key_a.time <= animation->time /*&& got_two && keys[j]._time >= a._t*/)
				{
					s.one = &key_a;
					if (got_two)
						s.two = &keys[j];

					// todo, this needs work
					else if (animation->keyframes->loop)
					{
						s.two = &keys[0];
					}

					break;
				}
			}
		}

		if (s.one && s.two)
		{
			float t_1 = s.one->time;
			float t_2 = s.two->time;
			// todo, does t_2 need negation here
			if (loop && t_2 < t_1)
				t_2 = t_2 - csp->C->stop_time;
			float ratio = (animation->time - t_2) / (t_2 - t_1) + 1.0f;
			s.ratio = ratio <= 0 ? 0 : ratio >= 1 ? 1 : ratio;
		}

		return s;
	}

	void Animation::Step()
	{
		float adv = 1.f / 60.f;

		if (play)
			time += adv;

		if (time >= keyframes->csp->C->stop_time)
			time -= keyframes->csp->C->stop_time;

		//printf("time %f\n", time);

		SimpleNonInterpolated();

		skeleton->baseBone->group->Update();
		//printf("cbp %i", cbp->controller);
	}

	void Animation::SimpleNonInterpolated()
	{
		Nifp *model = keyframes->model;

		struct controlled_block_pointer *cbp;

		for (unsigned int i = 0; i < keyframes->csp->A->num_controlled_blocks; i++)
		{
			// Match node_name to a skeleton bone

			cbp = &keyframes->csp->controlled_blocks[i];

			char *name = nifp_get_string(model, cbp->node_name);

			auto has = skeleton->bones_named.find(name);
			if (has == skeleton->bones_named.end())
			{
				//printf("cant find bone %s\n", name);
				// cant find shield, weapon, quiver
				continue;
			}

			Bone *bone = has->second;

			ni_transform_interpolator_pointer *tip = (ni_transform_interpolator_pointer *)
				nifp_get_block(model, cbp->interpolator);

			ni_transform_data_pointer *tdp = (ni_transform_data_pointer *)
				nifp_get_block(model, tip->B->data);

			if (tip == NULL || tdp == NULL)
				continue;

			vec4 ro = Gloom_Vec_4(tip->transform->rotation);

			int num = tdp->A->num_rotation_keys;
			if (num)
			{
				for (int i = num - 1; i >= 0; i--)
				{
					auto key = &tdp->quaternion_keys[i];
					//printf("qk %i time %f\n", i, key->time);
					if (key->time <= time || num == 1)
					{
						ro = Gloom_Vec_4(key->value);
						break;
					}
				}
			}
			// else if (num == 1)
			// 	ro = *cast_vec_4((float *)(&tdp->quaternion_keys[0].value));

			vec3 tr = Gloom_Vec_3(tip->transform->translation);

			num = tdp->translations->num_keys;
			if (num)
			{
				for (int i = num - 1; i >= 0; i--)
				{
					auto key = &tdp->translation_keys[i];
					if (key->time <= time || num == 1)
					{
						//printf("tr time %f\n", key->time);
						tr = Gloom_Vec_3(key->value);
						break;
					}
				}
			}
			// else if (num == 1)
			// 	tr = Gloom_Vec_3(tdp->translation_keys[0].value);

			quat qu = quat(ro[0], ro[1], ro[2], ro[3]);

			mat4 matrix = mat4_cast(qu);
			matrix[3] = vec4(tr, 1);

			bone->mod = matrix;
			bone->group->matrix = matrix;
			bone->diff = inverse(matrix) * bone->rest;
			bone->group->Update();
		}
	}

} // namespace gloom

/*
int num = tdp->A->num_rotation_keys;
			if (num > 1)
			{
				auto res = interpolate<quaternion_key_pointer, vec4>(this, num, tdp->quaternion_keys);
				//printf(" res ratio %f\n", res.ratio);
				if (res.one && res.two)
				{
					vec4 q1 = Gloom_Vec_4(res.one->value);
					vec4 q2 = Gloom_Vec_4(res.two->value);
					// todo, what does this fix? yoyo fingrs?
					//if ((q1.asVec4() * q2.asVec4()) < 0)
					//	q1 = -q1;
					q = glm::slerp(quat(q1), quat(q2), res.ratio);
				}
			}
			// used for clutched hands
			else if (num == 1)
			{
				auto key = &tdp->quaternion_keys[0];
				if (key->time <= time)
					q = quat(Gloom_Vec_4(key->value));
			}

			m = glm::mat4_cast(q);

			vec3 v = Gloom_Vec_3(tip->transform->translation);

			num = tdp->translations->num_keys;
			if (num > 0)
			{
				auto res2 = interpolate<translation_key_pointer, vec3>(this, num, tdp->translation_keys);
				if (res2.one && res2.two)
				{
					const vec3 v1 = Gloom_Vec_3(res2.one->value) * (1.0f - res2.ratio);
					const vec3 v2 = Gloom_Vec_3(res2.two->value) * res2.ratio;
					v = v1 + v2;
				}
				else if (res2.one)
					v = Gloom_Vec_3(res2.one->value);
			}

			//if (_tween && bone->_v.length())
			//{
			// if (bone->_v.length())
			//v = (v * _t_tween) + (bone->_v_2 * (1.0f - _t_tween));
			//}
			//else
			//	bone->_v_2 = v;
			*/