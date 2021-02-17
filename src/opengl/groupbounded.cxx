#include <opengl/group.h>
#include <opengl/geometry.h>

GroupBounded::GroupBounded() : Group()
{
	axis = new Geometry;
	axis->SetupMesh();
}

void GroupBounded::Draw(const mat4 &model)
{
	Group::Draw(model);
	mat4 place = model * matrixWorld;
	if (axis && renderSettings.axes)
		axis->Draw(place);
}

void GroupBounded::Update()
{
	Group::Update();
	aabb = Aabb();
	if (geometry)
		aabb.extend(geometry->aabb);
	for (Group *child : groups)
	{
		GroupBounded *bounded = dynamic_cast<GroupBounded *>(child);
		if (bounded)
			aabb.extend(bounded->aabb);
	}
	obb = aabb;
	aabb = Aabb::mult(aabb, matrix);
}

DrawGroup::DrawGroup(Group *group, mat4 matrix) : group(group), matrix(matrix)
{
	Reset();
}

void DrawGroup::Reset()
{
	//group->Update();
	GroupBounded *bounded = dynamic_cast<GroupBounded *>(group);
	if (bounded)
	{
		obb = aabb = bounded->aabb;
	}
	aabb = Aabb::mult(aabb, matrix);
	aabb.geometrize();
	obb.geometrize();
	//group->Flatten(group);w
}

void DrawGroup::Draw()
{
	group->DrawBegin(matrix);
	mat4 place = matrix * group->matrix;
	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		aabb.geometry->Draw(mat4(1.0));
	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		obb.geometry->Draw(place);
}