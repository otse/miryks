#pragma once

#include <dark/dark.h>
#include <skyrim/skyrim.h>
#include <skyrim/record.h>

#include <renderer/renderer.h>
#include <renderer/aabb.h>

using namespace skyrim;

namespace dark
{
	class Reference;

	namespace Refs
	{
		extern Reference *handRef;
		extern std::vector<Reference *> labelled;
		extern bool labelingEnabled;
		extern vec3 projected;
		void Init();
		void Nearby();
		void Activate();
	};

	static void refs_init() { Refs::Init(); }
	
	// horrible superweight class
	class Reference : public Record
	{
	public:
		Container *container = nullptr;

		Reference(Record);
		~Reference();
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