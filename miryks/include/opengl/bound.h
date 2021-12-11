#ifndef OPENGL_BOUND_H
#define OPENGL_BOUND_H

// part of gloom

#include <opengl/aabb.h>

struct Bound
{
	Bound(group_type *);
	~Bound();
	
	void Update();
	void Draw(const mat4 &);

	group_type * const group;

	Aabb aabb, obb;
};

#endif