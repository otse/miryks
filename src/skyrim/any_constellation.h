#pragma once

#include <cstdarg>

#include <skyrim/grup.h>

// first attempt at templating the horrible grup.h code

// leave this here, it is bad

#if 0
namespace skyrim
{
struct any
{
	typedef grup<any> iter;
	typedef iter freeform;
	union {
		crecord *r;
		cgrup *g;
		esp_dud *v;
	} u;
	any() {}
	any(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{
		u.v = i.get<esp_dud *>();
	}
	bool match(const char *id)
	{
		return record(u.r).editor_id(id);
	}
};

struct constellation: any
{
	typedef grup<constellation> iter;
	record self;
	grup<any> childs;
	constellation() {}
	constellation(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{
		self = i.record();
		childs = i.grupany();
	}
	bool match(const char *id)
	{
		return self.editor_id(id);
	}
};

struct cell
{
	const constellation cons;
	cell(const constellation &c) : cons(c)
	{

	}
};

struct wrld
{
	const constellation cons;
	wrld(const constellation &c) : cons(c)
	{

	}
};
}
#endif