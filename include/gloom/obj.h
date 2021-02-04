#pragma once

#include <Gloom/Dark2.h>

#include <opengl/aabb.h>
#include <opengl/types.h>

namespace gloom
{
	class Obj
	{
	public:
		Obj();
		~Obj();

		mat4 matrix;

		Mesh *mesh;
		DrawGroup *drawGroup;
	};
} // namespace gloom