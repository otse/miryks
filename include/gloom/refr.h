#pragma once

#include <gloom/dark2.h>

#include <opengl/aabb.h>
#include <opengl/types.h>

namespace gloom
{
	class Ref
	{
	public:
		Ref(record *);
		~Ref();

		Object *self, *base;

		mat4 matrix;

		AABB aabb;

		Mesh *mesh;
		Renderable *renderable;
		PointLight *pointlight;

		void Go();

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace gloom