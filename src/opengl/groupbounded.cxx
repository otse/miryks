#include <OpenGL/Group.h>
#include <OpenGL/Geometry.h>

GroupBounded::GroupBounded() : Group()
{
	aabb = Aabb();
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
	aabb = Aabb();
	Group::Update();
	// Once everything is updated,
	// Run back to front
	if (geometry)
		aabb.extend(geometry->aabb);
	aabb = Aabb::mult(aabb, matrix);
	if (parent)
	{
		GroupBounded *bounded = dynamic_cast<GroupBounded *>(parent);
		if (bounded)
			bounded->aabb.extend(aabb);
	}
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
		aabb = Aabb::mult(aabb, matrix);
		obb.geometrize();
		aabb.geometrize();
	}
	//group->Flatten(group);
}

void DrawGroup::Draw()
{
	group->Draws(matrix);
	mat4 place = matrix * group->matrix;
	if (renderSettings.AABBS && aabb.geometry && aabb.volume() <= renderSettings.maximumBoundingVolume)
		aabb.geometry->Draw(mat4(1.0));
	if (renderSettings.OBBS && obb.geometry && aabb.volume() <= renderSettings.maximumBoundingVolume)
		obb.geometry->Draw(place);
}