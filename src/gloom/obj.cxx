#include <Gloom/Obj.h>

#include <Attic.hpp>

namespace gloom
{
	Objs *globalObjs;

	void objs_init()
	{
		globalObjs = new Objs;
	}

	void Objs::Add(Obj *obj)
	{
		gloom::Add<Obj *>(obj, objs);
	}

	void Objs::Remove(Obj *obj)
	{
		gloom::Remove<Obj *>(obj, objs);
	}

	Obj::Obj()
	{
	}

	Obj::Obj(Ref *ref) : ref(ref)
	{
		
	}
} // namespace gloom