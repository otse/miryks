#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

DrawGroup::DrawGroup(Group *group, mat4 matrix)
	: group(group), matrix(matrix)
{
	Reset();
}

void DrawGroup::Reset()
{
	Rebound();
}

void DrawGroup::Rebound()
{
	GroupBounded *bounded = dynamic_cast<GroupBounded *>(group);
	if (bounded)
	{
		obb = aabb = bounded->aabb;
		aabb = Aabb::mult(aabb, matrix);
		obb.geometrize();
		aabb.geometrize();
	}
}

void DrawGroup::Draw()
{
	group->DrawMultiple(matrix);
	DrawBounds();
}

void DrawGroup::DrawBounds()
{
	mat4 place = matrix * group->matrix;
	bool notTooLarge = aabb.volume() <= renderSettings.maximumBoundingVolume;
	if (renderSettings.AABBS && aabb.geometry && notTooLarge)
		aabb.geometry->Draw(mat4(1.0));
	if (renderSettings.OBBS && obb.geometry && notTooLarge)
		obb.geometry->Draw(place);
}

float DrawGroup::GetZ() const
{
	return -vec3((cameraCur->view * matrix)[3]).z;
}