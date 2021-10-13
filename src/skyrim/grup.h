#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{
	// todo cut grup in two
	// one will be a non specialized base class
	// second argument: everything is inline anyway

	template <typename T = any>
	struct Grup
	{
		typedef std::function<bool(T &)> specialized_func;
		inline static specialized_func match_func = [](T &) -> bool {
			return false;
		};
		cgrup *me = nullptr;
		size_t index = 0;
		Grup()
		{
		}
		Grup(const char *word) : Grup(esp_top(get_plugins()[5], word))
		{
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
			return me;
		}
		inline T typesake()
		{
			return T(*this);
		}
		inline cgrup *pointer() const
		{
			return me;
		}
		inline const cgrup_header &hed() const
		{
			return *me->hed;
		}
		inline const revised_array &mixed() const
		{
			return *me->mixed;
		}
		inline bool going() const
		{
			return index < mixed().size;
		}
		bool loop(specialized_func func, int group_type = -1)
		{
			assertc(valid());
			assertc(group_type == -1 || hed().group_type == group_type);
			while (going()) {
				T temp = typesake();
				if (func(temp))
					return true;
			}
			return false;
		}
		bool match(const char *id, int group_type = -1)
		{
			return loop([]{
				return typesake().match(id);
			}, group_type);
		}
		bool dive(specialized_func func, std::vector<int> group_types, int dive = -1)
		{
			assertc(valid());
			if (dive == -1)
				dive = group_types.size();
			int group_type = group_types[group_types.size() - dive];
			if (dive == 1)
				return loop(func, group_type);
			else
				while (going())
					if(grup().dive(func, group_types, dive - 1))
						return true;
			return false;
		}
		template <typename T = void *>
		inline T get()
		{
			if (index >= mixed().size)
				return nullptr;
			return (T)mixed().elements[index++];
		}
		inline Grup<any> grupany()
		{
			printf("grupany\n");
			return get<cgrup *>();
		}
		inline Grup<T> grup()
		{
			printf("grup\n");
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