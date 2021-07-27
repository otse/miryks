#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

// this wraps a "lib struct"

namespace skyrim
{
	class Record;

	class Grup
	{
	public:
		const grup *grp;

		bool valid() const
		{
			return grp != nullptr;
		}

		Grup()
		{
			grp = nullptr;
		}

		Grup(const grup *grp)
		{
			(*this)(grp);
		}

		Grup &operator()(const grup *p)
		{
			grp = p;
			assertc(grp->g == 103);
			return *this;
		}

		const grup_header &hed() const
		{
			return *grp->hed;
		}

		const unsigned int size() const
		{
			return grp->mixed->size;
		}

		bool xtype(unsigned int i, char x) const
		{
			return x == (*(esp_dud ***)grp->mixed)[i]->x;
		}

		void foreach(int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < grp->mixed->size; i++)
				if (f(i))
					break;
		}
		
		template <typename T = void *>
		T get(unsigned int i, char x = '\0') const
		{
			assertc(i < grp->mixed->size);
			assertc(x == '\0' || xtype(i, x));
			return (*(T **)grp->mixed)[i];
		}

		const grup *getgrup(unsigned int i) const
		{
			return get<const grup *>(i, 'g');
		}

		const record *getrecord(unsigned int i) const
		{
			return get<const record *>(i, 'r');
		}

		/*
		Grup getGrup(unsigned int i) const
		{
			return Grup(getgrup(i));
		}

		Record getRecord(unsigned int i) const
		{
			return getrecord(i);
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