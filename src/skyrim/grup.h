#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

namespace skyrim
{
	// wrap for lib struct see /lib

#define X Grup

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
		void foreach (int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < mixed().size; i++)
				if (f(i))
					break;
		}
		template <typename T = void *>
		T get(unsigned int i, char x = '\0') const
		{
			assertc(i < mixed().size);
			assertc(x == '\0' || x == xtype(i));
			return (*(T **)&mixed())[i];
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

		char xtype(unsigned int i) const
		{
			return (*(esp_dud ***)&mixed())[i]->x;
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