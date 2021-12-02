#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{

	/*
	experimental templated-grup-contracts,
	read at your own risk
	*/

#define passthru <=

	struct any;

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
		typedef cgrup *grup_low;
		typedef crecord *record_low;
		typedef esp_dud *low_any;

		grup_low g;
		unsigned int index;
		grup_basic()
		{
			g = nullptr;
		}
		grup_basic(
			grup_low low)
		{
			set(low);
		}
		void set(grup_low low)
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
		low_any get_next()
		{
			return (low_any)mixed().elements[index++];
		}
		grup_low next_grup()
		{
			return (grup_low)get_next();
		}
		record_low next_record()
		{
			return (record_low)get_next();
		}
		const cgrup_header &hed() { return *g->hed; }
		const revised_array &mixed() { return *g->mixed; }
	};

	struct grup_top : grup_basic
	{
		grup_top(int plugin, const char *top)
			: grup_basic(esp_top(get_plugins()[plugin], top))
		{
			printf("grup_top\n");
		}
	};

	struct any
	{
		iterator *it = nullptr;
		any()
		{
		}
		any(iterator &i)
		{
			it = &i;
		}
		template <typename deduced>
		bool operator passthru(deduced &rhs)
		{
			if (std::is_same<any, deduced>::value)
				printf("forever");
			return deduced(*it) <= (rhs);
		}
	};

	typedef any here;

	template <int intended_group_type = -1, typename next = any>
	struct grup : grup_basic
	{
		typedef grup_basic implicit_downcast;
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
		void operator=(const implicit_downcast &rhs)
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
		typedef record_basic implicit_downcast;
		record()
		{
		}
		record(iterator &i)
		{
			operator=(i.next_record());
		}
		void operator=(const implicit_downcast &rhs)
		{
			this->set(rhs.r);
		}
	};

	template <typename next>
	struct closure : any
	{
		typedef next T;
		T match;
		void *pointer = nullptr;
		closure(void *pass) : pointer(pass)
		{
		}
		closure(const char *pass) : pointer((void *)pass)
		{
		}
		template <typename deduced>
		bool operator passthru(deduced &rhs)
		{
			while (this->it->under())
			{
				T temp = T(*this->it);
				if (temp <= rhs)
				{
					rhs.match = temp;
					return true;
				}
			}
			return false;
		}
		using any::any;
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

	template <int T, typename Y>
	using Grup = grup<T, Y>;
	template <typename T>
	using Closure = closure<T>;
	typedef any Any;
	typedef grup_basic GrupBasic;
	typedef grup_top GrupTop;
	typedef record_basic RecordBasic;
	typedef record_and_grup RecordAndGrup;

	static const char *const cells = "CELL";
	static const char *const races = "RACE";

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