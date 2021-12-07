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
		grup(const grup &other)
		{
			g = other.g;
		}
		grup(iterator &i)
		{
			operator=(i.next_grup());
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
		template <typename T>
		inline T set_user_data(void *user_data)
		{
			grup_closure<T> target(user_data);
			*this <= target;
			return target.hit;
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

	struct grup_cap
	{
		iterator *save;
		grup_cap()
		{
		}
		grup_cap(iterator &i)
		{
			save = &i;
		}
		template <typename T>
		bool fat_arrow(T &target)
		{
			return T(*save) <= target;
		}
	};
	
	template <typename next>
	struct grup_closure
	{
		next hit;
		void *user_data = nullptr;
		iterator *const save = nullptr;
		grup_closure(void *user_data) : user_data(user_data)
		{
		}
		grup_closure(iterator &i) : save(&i)
		{
		}
		template <typename T>
		bool fat_arrow(T &target)
		{
			while (save->under())
			{
				next temp(*save);
				if (temp <= target)
				{
					target.hit = temp;
					return true;
				}
			}
			return false;
		}
	};

	struct record_and_grup : record, grup<>
	{
		record_and_grup()
		{
		}
		record_and_grup(iterator &iterator) : record(iterator), grup<>(iterator)
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
			return this->editor_id((const char *)target.user_data);
		}
	};

	struct record_with_id_and_grup : record_and_grup
	{
		bool fat_arrow(grup_closure<record_with_id_and_grup> &target)
		{
			return this->editor_id((const char *)target.user_data);
		}
	};

	template<typename runner, typename T>
	runner run_struct_on_grup(T &g, void *user_data)
	{
		return g.set_user_data<runner>(user_data);
	}

	template <typename T>
	record_and_grup find_record_with_id_and_grup(const char *id, T g)
	{
		return run_struct_on_grup<record_with_id_and_grup>(g, (void *)id);
	}

	template <typename T>
	record find_record_with_id(const char *id, T g)
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