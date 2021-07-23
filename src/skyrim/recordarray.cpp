#include <lib.h>

#include "record.h"
#include "recordarray.h"

namespace skyrim
{
	#define X ObjectArray

	X::X()
	{
		(*this)(nullptr); // calls operator!
	};

	X::X(Grup *grup)
	{
		assert(grup);
		(*this)(grup);
	}

	X &X::operator()(Grup *grup)
	{
		this->grup = grup;
		return *this;
	}

	unsigned int X::amount() const
	{
		return grup->mixed.size;
	}

	Grup *X::getgrup(unsigned int i) const
	{
		return get<Grup *>(i, GRUP);
	}

	Record *X::getrecord(unsigned int i) const
	{
		return get<Record *>(i, RECORD);
	}

	Object X::getobject(unsigned int i) const
	{
		return Object(getrecord(i));
	}

	ObjectArray X::getobjectarray(unsigned int i) const
	{
		return ObjectArray(getgrup(i));
	}
	#undef X
}