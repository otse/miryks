#pragma once

// part of gloom

#include <dark/dark.h>

#include <renderer/Aabb.h>
#include <renderer/types.h>

namespace dark
{
	class Objs;
	class Obj;

	extern Objs *globalObjs;

	void objs_init();

	class Objs
	{
	public:
		std::vector<Obj *> objs;

		void Add(Obj *);
		void Remove(Obj *);
	};
	class Obj
	{
	public:
		Ref *const ref = nullptr;
		Obj();
		Obj(Ref *);

		~Obj();

		mat4 matrix;

		Mesh *mesh = nullptr;
		
		DrawGroup *drawGroup = nullptr;
	};
} // namespace dark