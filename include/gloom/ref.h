#pragma once

#include <Gloom/Dark2.h>

#include <opengl/aabb.h>
#include <opengl/types.h>

namespace gloom
{
	class Ref
	{
	public:
		Ref(Record *);
		~Ref();

		Object *self, *baseObject;

		mat4 matrix;

		Mesh *mesh;
		DrawGroup *drawGroup;
		PointLight *pointlight;

		void Go();

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace gloom