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

		aabb = obb = group->geometry->aabb;
	
	else
		
		aabb = obb = AABB(0);

	for (Group *child : group->groups)
	{
		child->bound->Update();
		aabb.extend(child->bound->aabb);
	}

}

void Bound::Draw()
{
	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
	
		aabb.draw(mat4(1.0));

	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)

		obb.draw(group->matrix);
}