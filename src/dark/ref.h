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
		extern std::map<unsigned int, DrawGroup *> wordGroups;
		extern bool labelingEnabled;
		extern vec3 projected;
		void Init();
		void Nearby();
		void Activate();
	};

	static void refs_init() { Refs::Init(); }
	
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
		Group *refGroup;
		DrawGroup *drawGroup = nullptr;
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