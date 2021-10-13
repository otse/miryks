#pragma once

#include <cstdarg>

#include <skyrim/grup.h>

namespace skyrim
{
struct any
{
	typedef grup<any> iter;
	typedef iter freeform;
	union {
		crecord *r;
		cgrup *g;
	} u;
	any() {}
	any(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{

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