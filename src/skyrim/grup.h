#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

namespace skyrim
{
	class Record;

	class Grup
	{
	public:
		const grupp grp;

		Grup()
		{
			(*this)(nullptr);
		}

		Grup(grupp grp)
		{
			assertc(grp);
			(*this)(grp);
		}

		Grup &operator()(grupp grp)
		{
			this->grp = grp;
			return *this;
		}

		const grup_header &hed() const
		{
			return *grp->hed;
		}

		unsigned int amount() const
		{
			return grp->mixed.size;
		}

		bool typex(unsigned int i, int type) const
		{
			return type == ((Dud *)grp->mixed.elements[i])->x;
		}

		void foreach(int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < amount(); i++)
				if (f(i))
					break;
		}
		
		template <typename T = void *>
		T get(unsigned int i, int x = -1) const
		{
			assertc(i < amount());
			assertc(i != -1 && typex(i, x));
			return (T)grp->mixed.elements[i];
		}

		grupp getgrup(unsigned int i) const
		{
			return get<grupp>(i, GRUP);
		}

		recordp getrecord(unsigned int i) const
		{
			return get<recordp>(i, RECORD);
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