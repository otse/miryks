#pragma once

// part of gloom

#include <dark/dark.h>

#include <skyrim/record.h>

#include <renderer/types.h>

#include <renderer/Aabb.h>

namespace dark
{
	class Ref
	{
	public:
		Ref(::Record *);
		~Ref();
		const char *editorId = nullptr;
		Object selfObject, baseObject;
		mat4 translation, rotation, scale;
		mat4 matrix;
		Mesh *mesh;
		DrawGroup *drawGroup;
		PointLight *pointLight;
		SpotLight *spotLight;
		void go();
		void forScale(float *);
		void forLocationalData(float *);
		void forBaseId(unsigned int *);
		float getDistance();
		bool displayAsItem();
	};
} // namespace dark