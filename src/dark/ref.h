#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>
#include <skyrim/record.h>

#include <renderer/types.h>
#include <renderer/aabb.h>

using namespace skyrim;

namespace dark
{
	class Ref;

	namespace Refs
	{
		extern Ref *handRef;
		extern std::vector<Ref *> labelled;
		void Nearby();
		void Activate();
	};
	
	class Ref : public Record
	{
	public:
		Container *container = nullptr;

		Ref(crecordp);
		~Ref();
		Record baseObject;
		mat4 translation, rotation, scale;
		mat4 matrix;
		Mesh *mesh;
		DrawGroup *drawGroup;
		PointLight *pointLight;
		SpotLight *spotLight;
		void go();
		void forScale(float *);
		void forLocationalData(float *);
		void forBaseId(formId);
		float getDistance() const;
		bool displayAsItem();
		void step();
		bool Use();
	};
} // namespace dark