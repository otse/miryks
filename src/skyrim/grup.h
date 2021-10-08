#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

#define GET_JUST_RETURNS_NULLPTR 1

namespace skyrim
{
	// template doesnt do nothing yet :))
	
	template <typename T = Both>
	class Grup
	{
	public:
		typedef std::function<bool(Grup &)> loop_func;
		cgrup *me = nullptr;
		size_t index = 0;
		Grup() {
		}
		Grup(cgrup *grup) : me(grup)
		{
			if (grup)
			{
				assertc(grup->g == 'g');
				esp_check_grup(grup);
			}
		}
		inline bool valid() const
		{
			return me != nullptr;
		}
		inline const cgrup_header &hed() const
		{
			return *me->hed;
		}
		inline const revised_array &mixed() const
		{
			return *me->mixed;
		}
		Grup &next()
		{
			index++;
			return *this;
		}
		bool dive(loop_func func, std::vector<int> group_types, int dive = -1)
		{
			assertc(valid());
			if (dive == -1)
				dive = group_types.size();
			int group_type = group_types[group_types.size() - dive];
			if (loop(
					dive == 1 ? func : [&](Grup &g)
						{ return g.grup().dive(func, group_types, dive - 1); },
					group_type))
				return true;
			return false;
		}
		bool loop(loop_func func, int group_type = -1)
		{
			assertc(valid());
			assertc(group_type == -1 || hed().group_type == group_type);
			for (; index < mixed().size; index++)
				if (func(*this))
					return true;
			return false;
		}
		template <typename T = void *>
		inline T get()
		{
#if GET_JUST_RETURNS_NULLPTR
			if (index >= mixed().size)
				return nullptr;
#else
			assertc(i < size());
#endif
			return (T)mixed().elements[index];
		}
		inline Grup grup()
		{
			return get<cgrup *>();
		}
		inline Record record()
		{
			return get<crecord *>();
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