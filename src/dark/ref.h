#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>
#include <skyrim/record.h>

#include <renderer/renderer.h>
#include <renderer/aabb.h>

using namespace skyrim;

namespace dark
{
	class Ref;

	namespace Refs
	{
		extern Ref *handRef;
		extern std::vector<Ref *> labelled;
		extern bool labelingEnabled;
		extern vec3 projected;
		void Init();
		void Nearby();
		void Activate();
	};

	static void refs_init() { Refs::Init(); }
	
	// horrible superweight class
	class Ref : public Record
	{
	public:
		Container *container = nullptr;

		Ref(RCD);
		~Ref();
		Record baseObject;
		Model *model;
		mat4 matrix;
		mat4 translation, rotation, scale;
		Group *refGroup;
		DrawGroup *drawGroup = nullptr;
		PointLight *pointLight;
		SpotLight *spotLight;
		void Go();
		void ForScale(float *);
		void ForLocationalData(float *);
		void ForBaseId(formId);
		float GetDistance() const;
		bool DisplayAsItem();
		void Step();
		bool Use();
	};
} // namespace dark