#include <dark/Obj.h>

#include <attic.hpp>

namespace dark
{
	Objs *globalObjs;

	void objs_init()
	{
		globalObjs = new Objs;
	}

	void Objs::Add(Obj *obj)
	{
		VectorAdd<Obj *>(obj, objs);
	}

	void Objs::Remove(Obj *obj)
	{
		VectorRemove<Obj *>(obj, objs);
	}

	Obj::Obj()
	{
	}

	Obj::Obj(Ref *ref) : ref(ref)
	{
		
	}
} // namespace dark