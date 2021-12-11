#include <opengl/group.h>
#include <opengl/geometry.h>

GroupBounded::GroupBounded() : group_type()
{
	aabb = obb = Aabb();
	axis = new Geometry;
	axis->SetupMesh();
}

void GroupBounded::Update()
{
	aabb = Aabb();
	group_type::Update();
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