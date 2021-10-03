#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <functional>

// this lets you write cleaner code by pretending the access exists
#define OUT_OF_BOUNDS_GET_RETURNS_NULLPTR 1

namespace skyrim
{
	class Grup
	{
	public:
		typedef std::function<bool(unsigned int &)> loop_func;

		const GRUP grp;

		Grup()
		{
			grp = nullptr;
		}
		Grup(const GRUP p)
		{
			grp = p;
			(*this)(grp);
		}
		Grup(const Grup &grupw)
		{
			grp = grupw.grp;
			(*this)(grp);
		}
		Grup &operator()(const GRUP p)
		{
			grp = p;
			assertc(grp->g == 'g');
			esp_check_grup((GRUP)grp);
			
			return *this;
		}
		inline bool valid() const
		{
			return grp != nullptr;
		}
		inline const grup_header &hed() const
		{
			return *grp->hed;
		}
		inline const revised_array &mixed() const
		{
			return *grp->mixed;
		}
		bool loop(loop_func f, int group_type = -1)
		{
			assertc(valid());
			assertc(group_type == -1 || hed().group_type == group_type);
			for (unsigned int i = 0; i < mixed().size; i++)
				if (f(i))
					return 1;
			return 0;
		}
		protected:
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
		public:
		inline const grup *get_grup(unsigned int i) const
		{
			return get<grup *>(i);
		}
		inline const record *get_record(unsigned int i) const
		{
			return get<record *>(i);
		}
		
		// type safety comment-outs
		/*
		char xtype(unsigned int i) const
		{
			return *(char *)mixed().elements[i];
		}
		*/
	};

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