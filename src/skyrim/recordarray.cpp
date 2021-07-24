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
		assertc(grup);
		(*this)(grup);
	}

	X &X::operator()(Grup *grup)
	{
		this->grup = grup;
		return *this;
	}

	const grup_header &X::hed() const
	{
		return *grup->hed;
	}

	bool X::type(unsigned int i, int type) const
	{
		return type == ((Dud *)grup->mixed.elements[i])->x;
	}

	unsigned int X::amount() const
	{
		return grup->mixed.size;
	}

	void X::foreach (int group_type, std::function<bool(unsigned int &i)> f)
	{
		assertc(hed().group_type == group_type);
		for (unsigned int i = 0; i < amount(); i++)
			if (f(i))
				break;
	}
#undef X
}