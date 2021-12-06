#pragma once

#include <cstdarg>

#include <miryks/record.h>

#include <map>
#include <array>
#include <functional>

namespace miryks
{
	/*
	experimental templated-grup-contracts,
	read at your own risk!!
	*/

#define fat_arrow operator <=

	struct destination;

	template <int, typename>
	struct grup;
	struct grup_basic;
	template <typename>
	struct grup_top;

	struct record;
	struct record_basic;

	template <typename>
	struct grup_closure;

	struct record_with_id;
	struct record_with_id_and_grup;

	typedef grup_basic iterator;

	struct grup_basic
	{
		typedef cgrup *c_grup;
		typedef crecord *c_record;
		typedef esp_dud *c_dud;

		c_grup g = nullptr;
		unsigned int index = 0;
		grup_basic()
		{
		}
		grup_basic(c_grup low)
		{
			set(low);
		}
		void set(c_grup low)
		{
			g = low;
			if (g)
			{
				assertc(g->g == 'g');
				esp_check_grup(g);
				rewind();
			}
		}
		bool valid()
		{
			return g;
		}
		void rewind()
		{
			index = 0;
		}
		bool under()
		{
			return index < mixed().size;
		}
		template <typename T>
		T get(int i)
		{
			return reinterpret_cast<T>(mixed().elements[i]);
		}
		c_grup next_grup()
		{
			return get<c_grup>(index++);
		}
		c_record next_record()
		{
			return get<c_record>(index++);
		}
		const cgrup_header &hed()
		{
			return *g->hed;
		}
		const revised_array &mixed()
		{
			return *g->mixed;
		}
	};

	template <typename next = destination>
	struct grup_top : grup<0, next>
	{
		grup_top(const char *top, int plugin)
		{
			this->set(esp_top(get_plugins()[plugin], top));
		}
	};

	template <int intended_group_type = -1, typename next = destination>
	struct grup : grup_basic
	{
		typedef grup_basic basic_cast;
		typedef next T;
		T last;
		void *some_value;
		grup(const grup &) = delete;
		grup()
		{
		}
		grup(iterator &i)
		{
			operator=(i.next_grup());
		}
		void operator=(const basic_cast &rhs)
		{
			this->set(rhs.g);
			int group_type = this->hed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		template <typename anything>
		bool fat_arrow(anything &rhs)
		{
			while (this->under())
				if (T(*this) <= rhs)
					return true;
			return false;
		}
		template <typename type>
		type &perform(void *some_value)
		{
			grup_closure<type> capture(some_value);
			*this <= capture;
			return capture.match;
		}
		record_with_id_and_grup &find_record_with_id_and_grup(const char *id)
		{
			return perform<record_with_id_and_grup>((void*)id);
		}
		record &find_record_with_id(const char *id)
		{
			return perform<record_with_id>((void *)id);
		}
	};

	struct record : record_basic
	{
		typedef record_basic basic_cast;
		record()
		{
		}
		record(iterator &i)
		{
			operator=(i.next_record());
		}
		void operator=(const basic_cast &rhs)
		{
			this->set(rhs.r);
		}
	};

	struct destination
	{
		iterator *save;
		destination() {}
		destination(iterator &i)
		{
			save = &i;
		}
		template <typename do_whatever>
		bool fat_arrow(do_whatever &rhs)
		{
			return do_whatever(*save) <= rhs;
		}
	};

	// todo goal is to refractor closure into destination
	// but this is difficult
	template <typename next>
	struct grup_closure
	{
		typedef next T;
		iterator *save;
		T match;
		void *pointer = nullptr;
		grup_closure(iterator &i)
		{
			save = &i;
		}
		grup_closure(void *some_value)
		{
			pointer = some_value;
		}
		grup_closure(const char *editorId)
		{
			pointer = (void *)editorId;
		}
		template <typename anything>
		bool fat_arrow(anything &rhs)
		{
			while (save->under())
			{
				T temp = T(*save);
				if (temp <= rhs)
				{
					rhs.match = temp;
					return true;
				}
			}
			return false;
		}
	};

	struct record_with_id : record
	{
		bool fat_arrow(grup_closure<record_with_id> &rhs)
		{
			return this->editor_id((const char *)rhs.pointer);
		}
	};
	
	struct record_with_id_and_grup
	{
		record one;
		grup<> two;
		record_with_id_and_grup()
		{
		}
		record_with_id_and_grup(iterator &i)
		{
			one = i.next_record();
			two = i.next_grup();
		}
		record_with_id_and_grup(const record_with_id_and_grup &rhs)
		{
			one = rhs.one;
			two = rhs.two;
		}
		bool fat_arrow(grup_closure<record_with_id_and_grup> &rhs)
		{
			return one.editor_id((const char *)rhs.pointer);
		}
	};

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