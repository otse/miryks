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

	struct grup_destination;

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

	record &find_record_with_id(const char *);
	record_with_id_and_grup &find_record_with_id_and_grup(const char *);

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
		const cgrup_header &ghed()
		{
			return *g->hed;
		}
		const revised_array &mixed()
		{
			return *g->mixed;
		}
	};

	template <typename next = grup_destination>
	struct grup_top : grup<0, next>
	{
		grup_top(const char *top, int plugin)
		{
			this->set(esp_top(get_plugins()[plugin], top));
		}
	};

	template <int intended_group_type = -1, typename next = grup_destination>
	struct grup : grup_basic
	{
		typedef grup_basic basic_cast;
		typedef next T;
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
			int group_type = this->ghed().group_type;
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
		inline type &set_user_data(void *some_value)
		{
			grup_closure<type> capture(some_value);
			*this <= capture;
			return capture.match;
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

	struct grup_destination
	{
		iterator *save;
		grup_destination(iterator &i)
		{
			save = &i;
		}
		template <typename do_whatever>
		bool fat_arrow(do_whatever &rhs)
		{
			return do_whatever(*save) <= rhs;
		}
	};

	template <typename next>
	struct grup_closure
	{
		typedef next T;
		iterator *save;
		T match;
		void *some_value = nullptr;
		grup_closure(iterator &i)
		{
			save = &i;
		}
		grup_closure(void *some_value) : some_value(some_value)
		{
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
			return this->editor_id((const char *)rhs.some_value);
		}
	};

	struct record_with_id_and_grup : record, grup<>
	{
		record_with_id_and_grup()
		{
		}
		record_with_id_and_grup(iterator &i) : record(i), grup<>(i)
		{
		}
		record_with_id_and_grup(const record_with_id_and_grup &rhs)
		{
			record::operator=(rhs);
			grup<>::operator=(rhs);
		}
		bool fat_arrow(grup_closure<record_with_id_and_grup> &rhs)
		{
			return this->editor_id((const char *)rhs.some_value);
		}
	};

	template <typename deduced>
	record_with_id_and_grup &find_record_with_id_and_grup(deduced g, const char *id)
	{
		return g.set_user_data<record_with_id_and_grup>((void*)id);
	}

	template <typename deduced>
	record &find_record_with_id(deduced g, const char *id)
	{
		return g.set_user_data<record_with_id>((void *)id);
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