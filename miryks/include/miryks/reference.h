#pragma once

#include <miryks/miryks.h>
#include <miryks/record.h>

#include <opengl/renderer.h>
#include <opengl/aabb.h>

namespace miryks
{
	class reference;

	namespace itemfinder
	{
	extern bool detect;
	extern reference *handRef;
	extern std::vector<reference *> pool;
	extern vec3 projected;
	void init();
	void consider();
	void activate();
	}

	class reference : public record_copy
	{
	public:
		cell *cell = nullptr;
		mat4 matrix, translation, rotation, scale;

		reference(record_copy);
		~reference();
		Container *container = nullptr;
		record_copy baseObject;
		nifmodel *model;
		group_type *refGroup;
		drawgroup *drawGroup = nullptr;
		pointlight *pointLight;
		spotlight *spotLight;
		void Go();
		void ForScale(float *);
		void ForLocationalData(float *);
		void ForBaseId(formId);
		float GetDistance() const;
		bool DisplayAsItem();
		void Step();
		bool Use();
	};

	class light : reference
	{
	public:
	light(record_copy rc) : reference(rc)
	{
	}
	pointlight *pointLight;
	spotlight *spotLight;
	};
} // namespace dark