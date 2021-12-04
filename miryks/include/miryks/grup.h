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
	read at your own risk
	*/

#define passthru <=

	struct inflection;

	template <int, typename>
	struct grup;
	struct grup_basic;
	struct grup_top;
	struct record;
	struct record_basic;

	template <typename>
	struct closure;
	struct record_and_grup;

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

	struct grup_top : grup_basic
	{
		grup_top(int plugin, const char *top)
			: grup_basic(esp_top(get_plugins()[plugin], top))
		{
			printf("grup_top %.4s\n", top);
		}
	};

	template <int intended_group_type = -1, typename next = inflection>
	struct grup : grup_basic
	{
		typedef grup_basic up_or_downcast;
		typedef next T;
		grup(const grup &) = delete;
		grup()
		{
		}
		grup(const char *top, int plugin = 5)
		{
			operator=(grup_top(plugin, top));
		}
		grup(iterator &i)
		{
			operator=(i.next_grup());
		}
		void operator=(const up_or_downcast &rhs)
		{
			this->set(rhs.g);
			int group_type = this->hed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		template <typename deduced>
		bool operator passthru(deduced &rhs)
		{
			while (this->under())
				if (T(*this) <= rhs)
					return true;
			return false;
		}
	};

	struct record : record_basic
	{
		typedef record_basic up_or_downcast;
		record()
		{
		}
		record(iterator &i)
		{
			operator=(i.next_record());
		}
		void operator=(const up_or_downcast &rhs)
		{
			this->set(rhs.r);
		}
	};

	struct inflection
	{
		grup_basic *save;
		inflection(iterator &i)
		{
			save = &i;
		}
		template <typename deduced>
		bool operator passthru(deduced &rhs)
		{
			if (std::is_same<inflection, deduced>::value)
				printf("forever");
			return deduced(*save) <= rhs;
		}
	};

	template <typename next>
	struct closure
	{
		typedef next T;
		T match;
		grup_basic *save;
		void *pointer = nullptr;
		closure(iterator &i)
		{
			save = &i;
		}
		closure(void *pass) : pointer(pass)
		{
		}
		closure(const char *pass) : pointer((void *)pass)
		{
		}
		template <typename deduced>
		bool operator passthru(deduced &rhs)
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
		bool operator passthru(closure<record_with_id> &rhs)
		{
			return this->editor_id((const char *)rhs.pointer);
		}
	};

	struct record_and_grup
	{
		record one;
		grup<> two;
		record_and_grup()
		{
		}
		record_and_grup(iterator &i)
		{
			one = i.next_record();
			two = i.next_grup();
		}
		void operator=(const record_and_grup &rhs)
		{
			one = rhs.one;
			printf("try downcast on me\n");
			two = rhs.two;
		}
		bool operator passthru(closure<record_and_grup> &rhs)
		{
			return one.editor_id((const char *)rhs.pointer);
		}
	};

	static const char *const cells = "CELL";
	static const char *const races = "RACE";

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