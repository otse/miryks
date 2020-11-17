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

	Bone *Skeleton::Nested(nifprd *rd)
	{
		Bone *bone = new Bone();
		bones[rd->current] = bone;
		bones[rd->parent]->group->Add(bone->group);
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
		Bone *bone = skeleton->Nested(rd);
		matrix_from_common(bone, block->common);
	}

} // namespace gloom