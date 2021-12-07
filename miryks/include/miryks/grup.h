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

	struct grup_target;

	template <int, typename>
	struct grup;
	struct grup_basic;
	template <typename>
	struct grup_top;

	struct record;
	struct record_basic;

	template <typename>
	struct grup_closure;

	struct record_and_grup;
	struct record_with_id;
	struct record_with_id_and_grup;

	typedef grup_basic iterator;

	struct grup_basic
	{
		bool valid() { return g != nullptr; }
		cgrup *g = nullptr;
		unsigned int index = 0;
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

	template <typename next = grup_target>
	struct grup_top : grup<0, next>
	{
		grup_top(const char *top, int plugin)
		{
			this->set(esp_top(get_plugins()[plugin], top));
		}
	};

	template <int intended_group_type = -1, typename next = grup_target>
	struct grup : grup_basic
	{
		typedef grup_basic basic_cast;
		typedef next T;
		grup()
		{
		}
		grup(const grup &other)
		{
			g = other.g;
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
		bool fat_arrow(anything &target)
		{
			while (this->under())
				if (T(*this) <= target)
					return true;
			return false;
		}
		template <typename type>
		inline type set_user_data(void *some_value)
		{
			grup_closure<type> capture(some_value);
			*this <= capture;
			return capture.good_return;
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

	struct grup_target
	{
		iterator *save;
		grup_target(iterator &i)
		{
			save = &i;
		}
		template <typename do_whatever>
		bool fat_arrow(do_whatever &target)
		{
			return do_whatever(*save) <= target;
		}
	};

	// a capturing grup-target with a user pointer
	template <typename next>
	struct grup_closure
	{
		typedef next T;
		iterator *save;
		T good_return;
		void *some_value = nullptr;
		grup_closure(iterator &i)
		{
			save = &i;
		}
		grup_closure(void *some_value) : some_value(some_value)
		{
		}
		template <typename anything>
		bool fat_arrow(anything &target)
		{
			while (save->under())
			{
				T temp = T(*save);
				if (temp <= target)
				{
					target.good_return = temp;
					return true;
				}
			}
			return false;
		}
	};

	/*struct destination
	{
		void *some_value = nullptr;
		template <typename do_whatever>
		bool fat_arrow(do_whatever &target)
		{
			return do_whatever(*save) <= target;
		}
	};*/

	struct record_and_grup : record, grup<>
	{
		record_and_grup()
		{
		}
		record_and_grup(iterator &i) : record(i), grup<>(i)
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
		bool fat_arrow(grup_closure<record_pass> &target)
		{
			return false;
		}
	};

	struct record_with_id : record
	{
		bool fat_arrow(grup_closure<record_with_id> &target)
		{
			return this->editor_id((const char *)target.some_value);
		}
	};

	struct record_with_id_and_grup : record_and_grup
	{
		bool fat_arrow(grup_closure<record_with_id_and_grup> &target)
		{
			return this->editor_id((const char *)target.some_value);
		}
	};

	template<typename runner, typename deduced>
	runner run_struct_on_grup(deduced &g, void *some_value)
	{
		return g.set_user_data<runner>(some_value);
	}

	template <typename deduced>
	record_and_grup find_record_with_id_and_grup(const char *id, deduced g)
	{
		return run_struct_on_grup<record_with_id_and_grup>(g, (void *)id);
	}

	template <typename deduced>
	record find_record_with_id(const char *id, deduced g)
	{
		return run_struct_on_grup<record_with_id>(g, (void *)id);
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