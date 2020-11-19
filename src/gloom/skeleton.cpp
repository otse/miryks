#include <algorithm>

#include <gloom/libs>
#include <gloom/skeleton.h>

#include <opengl/group.h>
#include <opengl/types.h>

namespace gloom
{
	static void ni_node_callback(nifprd *, ni_node_pointer *);

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
		rc *rc = loadrc("meshes\\", ANAM, 0x1);

		nif = loadnifp(rc, 1);

		printf("num_blocks of skeleton %u\n", nif->hdr->num_blocks);
	}

	void Skeleton::Construct()
	{
		nifprd *rd = malloc_nifprd();
		rd->nif = nif;
		rd->data = this;
		//rd->other = other;
		rd->ni_node = ni_node_callback;
		nifp_rd(nif, rd, this);
		free_nifprd(&rd);
		baseBone->group->Update();
	}

	Bone *Skeleton::Nested(nifprd *rd, int name)
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
		// Slightly different from mesh code
		bone->group->matrix = translate(bone->group->matrix, *cast_vec_3((float *)&common->C->translation));
		bone->group->matrix *= glm::inverse(mat4((*cast_mat_3((float *)&common->C->rotation))));
		bone->group->matrix = scale(bone->group->matrix, vec3(common->C->scale));
	}

	void ni_node_callback(nifprd *rd, ni_node_pointer *block)
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

	// kf

	KeyFrames::KeyFrames(struct nifp *nif) : model(nif)
	{
		cassert(strcmp(model->hdr->block_types[0], NI_CONTROLLER_SEQUENCE) == 0, "block 0 not a controller sequence");

		csp = (ni_controller_sequence_pointer *)model->blocks[0];
	}

	// animation

	void Animation::Step()
	{
		float adv = 60.f / 1.f;

		if (time > kf->csp->C->stop_time)
			time -= kf->csp->C->stop_time;

		struct controlled_block_pointer *cbp;
		for (int i = 0; i < kf->csp->A->num_controlled_blocks; i++)
		{
			cbp = &kf->csp->controlled_blocks[i];
			// Match node_name to a skeleton bone

			auto has = skeleton->bones_named.find(nifp_get_string(kf->model, cbp->node_name));
			if (has == skeleton->bones_named.end())
				continue;
			
			
		}
		//printf("cbp %i", cbp->controller);
	}

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
	Lol<T, Y> interpolate(Animation *an, int num, const std::vector<T> &vector)
	{
		struct ni_controller_sequence_pointer *controllerSequence = an->model->blocks[0];

		cassert(strcmp(an->model->hdr->block_types[0], NI_CONTROLLER_SEQUENCE) == 0, "block 0 not a controller sequence");

		Lol<T, Y> s;
		s.one = nullptr;
		s.two = nullptr;
		s.ratio = 1;

		const auto loop = an->kf->loop;

		int i, j;
		//num = vector.size();
		if (num > 1)
		{
			i = num - (loop ? 1 : 2);

			for (; i >= 0; i--)
			{
				//loop:
				const T &key_a = vector[i];
				j = i + 1;
				const bool got_two = j <= num - 1;

				if (key_a._time <= an->time /*&& got_two && vector[j]._time >= a._t*/)
				{
					s.one = &key_a;
					if (got_two)
						s.two = &vector.at(j);

					// todo, this needs work
					else if (an->kf->loop)
					{
						s.two = &vector.at(0);
					}

					break;
				}
			}
		}

		if (s.one && s.two)
		{
			float t_1 = s.one->_time;
			float t_2 = s.two->_time;
			// todo, does t_2 need negation here
			if (loop && t_2 < t_1)
				t_2 = t_2 - controllerSequence._stop_time;
			float ratio = (an->time - t_2) / (t_2 - t_1) + 1.0f;
			s.ratio = ratio <= 0 ? 0 : ratio >= 1 ? 1 : ratio;
		}

		return s;
	}

} // namespace gloom