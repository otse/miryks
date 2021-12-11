#include <templates.hpp>

#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/geometry.h>
#include <opengl/material.h>
#include <opengl/camera.h>

#include <algorithm>

int drawgroup::num = 0;
int drawgroup::masks = ~0;

drawgroup::drawgroup(group_type *group, mat4 matrix)
	: target(group)
{
	this->matrix = matrix;
	mask = 1 << 0;
	parent = nullptr;
	num++;
	Reset();
	Update();
}

drawgroup::~drawgroup()
{
	num--;
}

void drawgroup::Reset()
{
	Cubify();
}

bool drawgroup::Invisible()
{
	if (visible && (drawgroup::masks & mask) == mask)
		return false;
	return true;
}

void drawgroup::Draw(const mat4 &left)
{
	if (Invisible())
		return;
	mat4 right = left * matrixWorld;
	if (childGroups.size())
		for (group_type *child : childGroups)
			if (dynamic_cast<drawgroup *>(child))
				child->Draw(right);
	if (target)
		target->DrawChilds(right);
	DrawBounds();
}

void drawgroup::Cubify()
{
	GroupBounded *bounded = dynamic_cast<GroupBounded *>(target);
	if (bounded)
	{
		obb = aabb = bounded->aabb;
		aabb = Aabb::mult(aabb, matrix);
		obb.geometrize();
		aabb.geometrize();
	}
}

void drawgroup::DrawBounds()
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