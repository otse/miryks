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

		unsigned int index = 0;
		const GRUP grp;
		Grup()
		{
			grp = nullptr;
		}
		Grup(GRUP grp)
		{
			(*this)(grp);
		}
		Grup &operator()(GRUP grp)
		{
			this->grp = grp;
			assertc(grp->g == 'g');
			esp_check_grup(grp);
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
			return get<grup *>();
		}
		inline Record get_record()
		{
			return get<record *>();
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