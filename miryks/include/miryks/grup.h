#pragma once

/// perhaps grups.h

#include <cstdarg>

#include <miryks/record.h>

#include <map>
#include <array>
#include <functional>

namespace miryks
{
	/*
	experimental template-grups

	read at your own risk
	*/

#define fat_arrow operator<=

	struct grup_cap;

	template <int, typename>
	struct grup;
	struct grup_basic;
	template <typename>
	struct grup_top;

	struct record;
	struct record_basic;

	struct record_and_grup;
	struct record_with_id;
	struct record_with_id_and_grup;

	struct grup_basic
	{
		cgrup *g = nullptr;
		unsigned int index = 0;
		bool valid()
		{
			return g != nullptr;
		}
		grup_basic()
		{
		}
		grup_basic(cgrup *low)
		{
			set(low);
		}
		const cgrup_header &ghed()
		{
			return *g->hed;
		}
		const revised_array &mixed()
		{
			return *g->mixed;
		}
		void set(cgrup *low)
		{
			g = low;
			if (g)
			{
				assertc(g->g == 'g');
				esp_check_grup(g);
				index = 0;
			}
		}
		bool under()
		{
			return index < mixed().size;
		}
		template <typename T>
		T child(int i)
		{
			return reinterpret_cast<T>(mixed().elements[i]);
		}
		cgrup *next_grup()
		{
			return child<cgrup *>(index++);
		}
		crecord *next_record()
		{
			return child<crecord *>(index++);
		}
	};

	template <typename next = grup_cap>
	struct grup_top : grup<0, next>
	{
		grup_top(const char *top, int plugin)
		{
			this->set(esp_top(get_plugins()[plugin], top));
		}
	};

	template <int intended_group_type = -1, typename next = grup_cap>
	struct grup : grup_basic
	{
		grup()
		{
		}
		//grup(const grup &other)
		//{
		//	g = other.g;
		//}
		grup(grup_basic &iterator)
		{
			operator=(iterator.next_grup());
		}
		void operator=(const grup_basic &rhs)
		{
			this->set(rhs.g);
			int group_type = this->ghed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		template <typename T>
		bool fat_arrow(T &target)
		{
			while (this->under())
				if (next(*this) <= target)
					return true;
			return false;
		}
	};

	struct grup_cap
	{
		grup_basic *iterator;
		grup_cap(grup_basic &iterator) : iterator(&iterator)
		{
		}
		template <typename T>
		bool fat_arrow(T &target)
		{
			T temp(*iterator);
			if (temp <= target)
			{
				target = temp;
				return true;
			}
			return false;
		}
	};

	struct record : record_basic
	{
		record()
		{
		}
		record(grup_basic &iterator)
		{
			operator=(iterator.next_record());
		}
		void operator=(const record_basic &rhs)
		{
			this->set(rhs.r);
		}
	};

	struct record_and_grup : record, grup<>
	{
		record_and_grup()
		{
		}
		record_and_grup(grup_basic &iterator) : record(iterator), grup<>(iterator)
		{
		}
		record_and_grup(const record_and_grup &rhs)
		{
			r = rhs.r;
			g = rhs.g;
		}
	};

	struct record_pass : record
	{
		bool fat_arrow(record_pass &target)
		{
			return false;
		}
	};

	struct record_with_id : record
	{
		const char *search = nullptr;
		bool fat_arrow(record_with_id &target)
		{
			return this->editor_id(target.search);
		}
	};

	struct record_with_id_and_grup : record_and_grup
	{
		const char *search = nullptr;
		bool fat_arrow(record_with_id_and_grup &target)
		{
			return this->editor_id(target.search);
		}
	};

	template <typename G>
	record_and_grup find_record_with_id_and_grup(const char *id, G grup)
	{
		record_with_id_and_grup target;
		target.search = id;
		grup <= target;
		return target;
	}

	template <typename G>
	record find_record_with_id(const char *id, G grup)
	{
		record_with_id target;
		target.search = id;
		grup <= target;
		return target;
	}

	enum GrupTypes
	{
		default_top = 0,
		world_children,
		interior_cell_block,
		interior_cell_subblock,
		exterior_cell_block,
		exterior_cell_subblock,
		cell_children,
		topic_children,
		cell_persistent_children,
		cell_temporary_children
	};
}