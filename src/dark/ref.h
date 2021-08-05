#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>
#include <skyrim/record.h>

#include <renderer/types.h>
#include <renderer/aabb.h>

using namespace skyrim;

namespace dark
{
	class Ref : public Record
	{
	public:
		Ref(crecordp);
		~Ref();
		Record baseObject;
		mat4 translation, rotation, scale;
		mat4 matrix;
		Mesh *mesh;
		DrawGroup *draw_group;
		PointLight *pointLight;
		SpotLight *spotLight;
		void go();
		void forScale(float *);
		void forLocationalData(float *);
		void forBaseId(formId);
		float getDistance();
		bool displayAsItem();
	};
} // namespace dark