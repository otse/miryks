#include <opengl/bound.h>

#include <opengl/group.h>
#include <opengl/geometry.h>

Bound::Bound(Group *tie) : group(tie)
{
	
}

Bound::~Bound()
{
	delete aabb.geometry;
	delete obb.geometry;
}

void Bound::Update()
{
	delete aabb.geometry;
	delete obb.geometry;

	if (group->geometry)

		aabb = group->geometry->aabb;
	
	else
		
		aabb = AABB();

	for (Group *child : group->groups)
	{
		child->bound->Update();
		aabb.extend(child->bound->aabb);
	}

	obb = aabb;

	aabb = AABB::mult(aabb, group->matrix);

	aabb.geometrize();
	obb.geometrize();
}

void Bound::Draw()
{
	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
	
		aabb.draw(mat4(1.0));

	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)

		obb.draw(group->matrix);
}