#pragma once

#include <dark/dark.h>
#include <skyrim/grup.h>

#include <renderer/types.h>
#include <renderer/aabb.h>

using namespace skyrim;

namespace dark
{
	class Ref
	{
	public:
		Ref(::record_t *);
		~Ref();
		const char *editorId = nullptr;
		Record selfObject, baseObject;
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