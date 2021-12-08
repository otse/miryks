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
	pt = passthru
	*/

#define fat_arrow operator<=

	struct record_basic;
	struct grup_basic;

	typedef record_basic record_copy;
	typedef grup_basic grup_copy;

	template <int, typename>
	struct pt_grup;
	struct pt_cap;
	struct pt_record;
	struct record_and_grup;
	struct pt_record_with_id;
	struct pt_record_with_id_and_grup;

	struct grup_basic
	{
		int maybe_intended_group_type = -1;
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

	template <int intended_group_type = -1, typename next = pt_cap>
	struct pt_grup : grup_basic
	{
		pt_grup()
		{
		}
		pt_grup(const char *top, int plugin)
		{
			operator=(esp_top(get_plugins()[plugin], top));
		}
		pt_grup(grup_basic &iterator)
		{
			operator=(iterator.next_grup());
		}
		void operator=(const grup_basic &rhs)
		{
			adopt(rhs);
		}
		void adopt(const grup_basic &rhs)
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

	struct pt_cap
	{
		grup_basic *iterator;
		pt_cap(grup_basic &iterator) : iterator(&iterator)
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

	struct pt_record : record_basic
	{
		pt_record()
		{
		}
		pt_record(grup_basic &iterator)
		{
			operator=(iterator.next_record());
		}
		void operator=(const record_basic &rhs)
		{
			this->set(rhs.r);
		}
	};

	struct record_and_grup : pt_record, pt_grup<>
	{
		record_and_grup()
		{
		}
		record_and_grup(grup_basic &iterator) : pt_record(iterator), pt_grup<>(iterator)
		{
		}
		record_and_grup(const record_and_grup &rhs)
		{
			r = rhs.r;
			g = rhs.g;
		}
	};

	struct pt_record_with_id : pt_record
	{
		const char *search = nullptr;
		bool fat_arrow(pt_record_with_id &target)
		{
			return this->editor_id(target.search);
		}
	};

	struct pt_record_with_id_and_grup : record_and_grup
	{
		const char *search = nullptr;
		bool fat_arrow(pt_record_with_id_and_grup &target)
		{
			return this->editor_id(target.search);
		}
	};

	template <typename T>
	record_and_grup find_record_with_id_and_grup(const char *id, T grup)
	{
		pt_record_with_id_and_grup target;
		target.search = id;
		grup <= target;
		return target;
	}

	template <typename T>
	record_copy find_record_with_id(const char *id, T grup)
	{
		pt_record_with_id target;
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