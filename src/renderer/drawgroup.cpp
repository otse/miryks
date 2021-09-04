#include <templates.hpp>

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

int DGNum = 0;
int DGMask = ~0;

DrawGroup::DrawGroup(Group *group, mat4 matrix)
	: target(group)
{
	this->matrix = matrix;
	mask = 1 << 0;
	parent = nullptr;
	DGNum++;
	ManualReset();
}
/*void DrawGroup::Add(Group *group)
{

}
void DrawGroup::Remove(Group *group)
{

}*/

DrawGroup::~DrawGroup()
{
	DGNum--;
}

void DrawGroup::ManualReset()
{
	Cubify();
}

bool DrawGroup::ShouldRender()
{
	if (!visible)
		return false;
	if (((DGMask & mask) == mask) == false)
		return false;
	return true;
}

void DrawGroup::DrawOverride(const mat4 &left)
{
	if (!ShouldRender())
		return;
	if (target)
		target->DrawChilds(matrix);
	//DrawChilds(matrix);
	//DrawBounds();
}

void DrawGroup::Cubify()
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