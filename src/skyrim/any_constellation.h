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
	} rg;
	any() {}
	any(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{
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
};

struct cell: constellation
{
	typedef grup<cell> iter;
	cell() {}
	cell(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{
		// custom behavior here
		self = i.record();
		childs = i.grupany();
	}
	static iter top()
	{
		return iter(CELL);
	}
};

struct wrld: constellation
{
	typedef grup<wrld> iter;
	wrld() {}
	wrld(iter &i)
	{
		operator=(i);
	}
	void operator=(iter &i)
	{
		self = i.record();
		childs = i.grupany();
	}
	static iter top()
	{
		return iter(WRLD);
	}
};
}