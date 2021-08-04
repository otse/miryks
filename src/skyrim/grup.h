#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

// this lets you write cleaner code by pretending the access exists
#define OUT_OF_BOUNDS_GET_RETURNS_NULLPTR 1

namespace skyrim
{
	typedef std::function<bool(unsigned int &i)> grupfunc;

	typedef bool(*grupfuncp)(unsigned int &i);

#define X Grup

	class Grup
	{
	public:
		cgrupp grp;

		X()
		{
			grp = nullptr;
		}
		X(cgrupp p)
		{
			grp = p;
			if (grp)
				(*this)(grp);
		}
		X &operator()(cgrupp p)
		{
			grp = p;
			assertc(grp->g == 'g');
			return *this;
		}
		inline bool valid() const
		{
			return !!grp;
		}
		inline const grup_header &hed() const
		{
			return *grp->hed;
		}
		inline const revised_array &mixed() const
		{
			return *grp->mixed;
		}
		int foreach (int group_type, grupfunc f)
		{
			assertc(valid());
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < mixed().size; i++)
				if (f(i))
					return 1;
			return 0;
		}
		template <typename T = void *>
		inline T get(unsigned int i/*, char x = '\0'*/) const
		{
#if OUT_OF_BOUNDS_GET_RETURNS_NULLPTR
			if (i >= mixed().size)
				return nullptr;
#else
			assertc(i < mixed().size);
#endif
			//assertc(x == '\0' || x == xtype(i));
			return (T)mixed().elements[i];
		}

		// type safety comment-outs

		/*
		cgrupp getgrup(unsigned int i) const
		{
			// just use get<grup * or record *>
			return get<grup *>(i, 'g');
		}
		crecordp getrecord(unsigned int i) const
		{
			return get<record *>(i, 'r');
		}
		// << bluh >>

		char xtype(unsigned int i) const
		{
			return *(char *)mixed().elements[i];
		}
		*/
	};

#undef X

	enum GrupTypes
	{
		Top = 0,
		WorldChildren,
		InteriorCellBlock,
		InteriorCellSubBlock,
		ExteriorCellBlock,
		ExteriorCellSubBlock,
		CellChildren,
		TopicChildren,
		CellPersistentChildren,
		CellTemporaryChildren
	};
}