#include <templates.hpp>

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

int DrawGroup::Num = 0;

DrawGroup::DrawGroup(Group *group, mat4 matrix)
	: target(group), matrix(matrix)
{
	toggle = true;
	parent = nullptr;
	Num++;
	Reset();
}

DrawGroup::~DrawGroup()
{
	Num--;
}

void DrawGroup::Add(DrawGroup *drawGroup)
{
	drawGroup->parent = this;
	vector_safe_add<DrawGroup *>(drawGroup, drawGroups);
}

void DrawGroup::Remove(DrawGroup *drawGroup)
{
	drawGroup->parent = nullptr;
	vector_safe_remove<DrawGroup *>(drawGroup, drawGroups);
}

void DrawGroup::Reset()
{
	Rebound();
}

void DrawGroup::Rebound()
{
	if (!target)
		return;
	GroupBounded *bounded = dynamic_cast<GroupBounded *>(target);
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
	if (!toggle)
		return;
	if (target)
		target->DrawRecursive(matrix);
	for (DrawGroup *drawGroup : drawGroups)
		drawGroup->Draw();
	DrawBounds();
}

void DrawGroup::DrawBounds()
{
	if (!target)
		return;
	mat4 place = matrix * target->matrix;
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