#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

// this lets you write cleaner code by pretending the access exists
#define GET_JUST_RETURNS_NULLPTR 1

namespace skyrim
{
	template <typename T = Both>
	class Grup
	{
	public:
		typedef std::function<bool(Grup &)> loop_func;

		unsigned int index = 0;
		GRP grp = nullptr;
		Grup()
		{
		}
		Grup(GRP grp)
		{
			(*this)(grp);
		}
		Grup(Grup &grp)
		{
			(*this)(grp.grp);
		}
		Grup &operator()(GRP grp)
		{
			this->grp = grp;
			if (grp)
			{
				assertc(grp->g == 'g');
				esp_check_grup(grp);
			}
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
		inline int group_type() const
		{
			return hed().group_type;
		}
		Grup &next()
		{
			index++;
			return *this;
		}
		// for blocks and subblocks you can dive..
		bool dive(loop_func func, std::vector<int> group_types, int dive = -1)
		{
			assertc(valid());
			if (dive == -1)
				dive = group_types.size();
			int group_type = group_types[group_types.size() - dive];
			if (loop(
					dive == 1 ? func : [&](Grup &g)
						{ return g.get_grup().dive(func, group_types, dive - 1); },
					group_type))
				return true;
			return false;
		}
		bool loop(loop_func func, int group_type = -1)
		{
			assertc(valid());
			assertc(group_type == -1 || hed().group_type == group_type);
			for (; index < size(); index++)
				if (func(*this))
					return true;
			return false;
		}
		template <typename T = void *>
		inline T get()
		{
#if GET_JUST_RETURNS_NULLPTR
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