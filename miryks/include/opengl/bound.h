#ifndef OPENGL_BOUND_H
#define OPENGL_BOUND_H

// part of gloom

#include <opengl/aabb.h>

struct Bound
{
	Bound(Group *);
	~Bound();
	
	void Update();
	void Draw(const mat4 &);

	Group * const group;

	Aabb aabb, obb;
};

#endif