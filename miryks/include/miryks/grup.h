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
	*/

#define fat_arrow operator<=

	struct record_copy;
	struct grup_copy;

	template <int, typename>
	struct grup;
	struct record;
	
	struct passthrough;
	struct record_and_grup;
	typedef record_and_grup rgrup_copy;
	struct record_with_id;
	struct record_with_id_and_grup;

	struct grup_copy
	{
		cgrup *g = nullptr;
		unsigned int index = 0;
		bool valid()
		{
			return !!g;
		}
		grup_copy()
		{
		}
		grup_copy(cgrup *p)
		{
			setg(p);
		}
		void setg(cgrup *p)
		{
			index = 0;
			g = p;
			esp_check_grup(g);
		}
		bool under()
		{
			return index < mixed().size;
		}
		const cgrup_header &ghed()
		{
			return *g->hed;
		}
		const revised_array &mixed()
		{
			return *g->mixed;
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

	template <int intended_group_type = -1, typename next = passthrough>
	struct grup : grup_copy
	{
		grup()
		{
		}
		grup(const char *top, int plugin)
		{
			operator=(esp_top(get_plugins()[plugin], top));
		}
		grup(grup_copy &iterator)
		{
			operator=(iterator.next_grup());
		}
		void operator=(const grup_copy &rhs)
		{
			this->setg(rhs.g);
			int group_type = this->ghed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		template <typename T>
		bool fat_arrow(T &rhs)
		{
			while (this->under())
				if (next(*this) <= rhs)
					return true;
			return false;
		}
	};

	static grup<> grup_upcast(grup_copy &other)
	{
		grup g;
		g = other;
		return g;
	}

	struct passthrough
	{
		grup_copy *iterator;
		passthrough(grup_copy &iterator) : iterator(&iterator)
		{
		}
		template <typename T>
		bool fat_arrow(T &rhs)
		{
			T temp(*iterator);
			if (temp <= rhs)
			{
				rhs = temp;
				return true;
			}
			return false;
		}
	};

	struct record : record_copy
	{
		record()
		{
		}
		record(grup_copy &iterator)
		{
			operator=(iterator.next_record());
		}
		void operator=(const record_copy &rhs)
		{
			this->setr(rhs.r);
		}
	};

	struct record_and_grup : record, grup<>
	{
		record_and_grup()
		{
		}
		record_and_grup(grup_copy &iterator) : record(iterator), grup<>(iterator)
		{
		}
		record_and_grup(const record_and_grup &rhs)
		{
			r = rhs.r;
			g = rhs.g;
		}
	};

	struct record_with_id : record
	{
		const char *search = nullptr;
		bool fat_arrow(record_with_id &rhs)
		{
			return this->editor_id(rhs.search);
		}
	};

	struct record_with_id_and_grup : record_and_grup
	{
		const char *search = nullptr;
		bool fat_arrow(record_with_id_and_grup &rhs)
		{
			return this->editor_id(rhs.search);
		}
	};

	template <typename T>
	record_and_grup find_record_with_id_and_grup(const char *id, T grup)
	{
		record_with_id_and_grup rhs;
		rhs.search = id;
		grup <= rhs;
		return rhs;
	}

	template <typename T>
	record_copy find_record_with_id(const char *id, T grup)
	{
		record_with_id rhs;
		rhs.search = id;
		grup <= rhs;
		return rhs;
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