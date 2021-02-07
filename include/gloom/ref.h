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

		const char *editorId = nullptr;

		Object *self = nullptr, *baseObject = nullptr;

		mat4 matrix;

		mat4 translation, rotation, scale;

		Mesh *mesh;
		DrawGroup *drawGroup;
		PointLight *pointlight;

		void Go();
		
		void forScale(float *);
		void forLocationalData(float *);
		void forBaseId(unsigned int *);

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace gloom