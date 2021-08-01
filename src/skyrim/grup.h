#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

#define inl inline

namespace skyrim
{
	class Record;

#define X Grup

	// wrap for lib struct see /lib

	class Grup
	{
	public:
		cgrupp grp;

		X()
		{
			grp = nullptr;
		}
		X(cgrupp grp)
		{
			(*this)(grp);
		}
		X &operator()(cgrupp p)
		{
			grp = p;
			assertc(grp->g == 'g');
			return *this;
		}
		inl bool valid() const
		{
			return !!grp;
		}
		inl const grup_header &hed() const
		{
			return *grp->hed;
		}
		inl const revised_array &ary() const
		{
			return *grp->mixed;
		}
		void foreach (int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < ary().size; i++)
				if (f(i))
					break;
		}
		template <typename T = void *>
		T get(unsigned int i, char x = '\0') const
		{
			assertc(i < ary().size);
			assertc(x == '\0' || xtype(i, x));
			return (*(T **)&ary())[i];
		}
		cgrupp getgrup(unsigned int i) const
		{
			return get<cgrupp>(i, 'g');
		}
		crecordp getrecord(unsigned int i) const
		{
			return get<crecordp>(i, 'r');
		}
		// << bluh >>

		bool xtype(unsigned int i, char x) const
		{
			return x == (*(esp_dud ***)&ary())[i]->x;
		}
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