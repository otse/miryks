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
		// static Grup &end;

		typedef std::function<bool(Grup &, unsigned int &)> loop_func;

		const GRUP grp;

		Grup()
		{
			grp = nullptr;
		}
		Grup(const GRUP grp)
		{
			(*this)(grp);
		}
		Grup &operator()(const GRUP grp)
		{
			this->grp = grp;
			assertc(grp->g == 'g');
			esp_check_grup((GRUP)grp);
			return *this;
		}
		Grup &operator()(const Grup grupw)
		{
			(*this)(grupw.grp);
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
		inline size_t size() const
		{
			return mixed().size;
		}
		bool loop(loop_func f, int group_type = -1, int dive = 0)
		{
			assertc(valid());
			printf("dive %i %i - %i\n", dive, group_type, hed().group_type);
			if (dive > 0) {
				for (unsigned int i = 0; i < size(); i++)
					if (get_grup(i).loop(f, group_type, dive - 1))
						return true;
			}
			else
				if (call(f, group_type))
					return true;
			return false;
		}
		protected:
		bool call(loop_func f, int group_type = -1)
		{
			printf("call %i - %i\n", group_type, hed().group_type);
			assertc(group_type == -1 || hed().group_type == group_type);
			for (unsigned int i = 0; i < size(); i++)
				if (f(*this, i))
					return true;
			return false;
		}
		template <typename T = void *>
		inline T get(unsigned int i) const
		{
#if OUT_OF_BOUNDS_GET_RETURNS_NULLPTR
			if (i >= size())
				return nullptr;
#else
			assertc(i < size());
#endif
			return (T)mixed().elements[i];
		}
		public:
		inline Grup get_grup(unsigned int i) const
		{
			return get<GRUP>(i);
		}
		inline Record get_record(unsigned int i) const
		{
			return get<RCD>(i);
		}
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