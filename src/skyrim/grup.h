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
			return !(grp == nullptr);
		}

		Grup()
		{
			(*this)(nullptr);
		}

		Grup(const grup *grp)
		{
			assertc(grp);
			(*this)(grp);
		}

		Grup &operator()(const grup *grp)
		{
			this->grp = grp;
			return *this;
		}

		const grup_header &hed() const
		{
			return *grp->hed;
		}

		bool xtype(unsigned int i, int type) const
		{
			return type == (*(esp_dud ***)grp->mixed)[i]->x;
		}

		void foreach(int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < grp->mixed->size; i++)
				if (f(i))
					break;
		}
		
		template <typename T = void *>
		T get(unsigned int i, int x = -1) const
		{
			assertc(i < grp->mixed->size);
			assertc(i != -1 && xtype(i, x));
			return (*(T **)grp->mixed)[i];
		}

		const grup *getgrup(unsigned int i) const
		{
			return get<const grup *>(i, GRUP);
		}

		const record *getrecord(unsigned int i) const
		{
			return get<const record *>(i, RECORD);
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