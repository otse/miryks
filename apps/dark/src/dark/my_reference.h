#pragma once

#include <dark/dark.h>
#include <dark/trash.h>

#include <miryks/miryks.hpp>

#include <miryks/model.h>

#include <opengl/renderer.h>
#include <opengl/aabb.h>

using namespace miryks;

namespace dark
{
	class my_reference;

	namespace itemfinder
	{
	extern bool detect;
	extern my_reference *handRef;
	extern std::vector<my_reference *> pool;
	extern vec3 projected;
	void init();
	void consider();
	void activate();
	}

	class my_reference : public reference
	{
	public:
		my_reference(record);
		~my_reference();
		
		Model *model;
		
		// mat4 matrix, translation, rotation, scale; // mir has

		//Container *container = nullptr;
		GroupDrawer *groupDrawer = nullptr;
		pointlight *pointLight;
		spotlight *spotLight;
		void Go();
		void ForScale(float *);
		void ForLocationalData(float *);
		void ForBaseId(const unsigned int *);
		float GetDistance() const;
		bool DisplayAsItem();
		void Step();
		bool use();
	};

	/*class light : reference
	{
	public:
	light(record rc) : reference(rc)
	{
	}
	pointlight *pointLight;
	spotlight *spotLight;
	};*/
} // namespace dark