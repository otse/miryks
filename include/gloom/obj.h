#pragma once

#include <Gloom/Dark2.h>

#include <OpenGL/Aabb.h>
#include <OpenGL/Types.h>

namespace gloom
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
} // namespace gloom