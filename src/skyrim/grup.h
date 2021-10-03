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
		typedef std::function<bool(Grup &)> loop_func;

		const GRUP grp;
		int index;
		Grup()
		{
			grp = nullptr;
			index = 0;
		}
		Grup(const GRUP grp) : Grup()
		{
			(*this)(grp);
		}
		Grup &operator()(const Grup grupw)
		{
			(*this)(grupw.grp);
			return *this;
		}
		Grup &operator()(const GRUP grp)
		{
			this->grp = grp;
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
		inline size_t size() const
		{
			return mixed().size;
		}
		Grup &next() {
			index++;
			return *this;
		}
		bool loop(loop_func f, int group_type = -1, int dive = 0)
		{
			assertc(valid());
			if (dive > 0) {
				printf("dive %i %i - %i\n", dive, group_type, hed().group_type);
				for (index = 0; index < size(); index++)
					if (get_grup().loop(f, group_type, dive - 1))
						return true;
			}
			else if (call(f, group_type))
				return true;
			return false;
		}
		protected:
		bool call(loop_func f, int group_type = -1)
		{
			printf("call %i %i - %i\n", index, group_type, hed().group_type);
			assertc(group_type == -1 || hed().group_type == group_type);
			for (index = 0; index < size(); index++)
				if (f(*this))
					return true;
			return false;
		}
		template <typename T = void *>
		inline T get()
		{
#if OUT_OF_BOUNDS_GET_RETURNS_NULLPTR
			if (index >= size())
				return nullptr;
#else
			assertc(i < size());
#endif
			return (T)mixed().elements[index];
		}
		public:
		inline Grup get_grup()
		{
			return get<GRUP>();
		}
		inline Record get_record()
		{
			return get<RCD>();
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