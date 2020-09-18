#pragma once

#include <gloom/dark2.h>

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

		Mesh *mesh = nullptr;
		Renderable *renderable = nullptr;
		PointLight *pointlight = nullptr;

		void Go();

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace gloom