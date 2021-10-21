#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{
	// passthru stuff
	// its not difficult

	template <int, typename>
	struct grup;
	struct grup_basic;
	struct grup_top;
	struct record;
	struct record_basic;

	template <typename>
	struct closure;
	struct record_and_grup;

	typedef grup_basic grup_high;
	typedef record_basic record_high;

	typedef grup_high iterator;

	typedef cgrup *grup_low;
	typedef crecord *record_low;

	struct grup_basic
	{
		typedef esp_dud *low_any;

		grup_low g;
		unsigned int index;
		grup_basic()
		{
			g = nullptr;
		}
		grup_basic(grup_low low)
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
		grup_top(const char *top) : grup_basic(esp_top(get_plugins()[5], top))
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
		// passthru
		template <typename deduced>
		bool operator()(deduced &target)
		{
			return deduced(*it)(target);
		}
	};

	template <int intended_group_type = -1, typename next = any>
	struct grup : grup_basic
	{
		const char *top = 0;
		grup()
		{
		}
		grup(const char *id)
		{
			top = id;
		}
		grup(iterator &i)
		{
			(*this) = i.next_grup();
		}
		// dont ask
		void operator|=(const char *top)
		{
			(*this) = grup_top(top);
		}
		void operator=(const grup_high &rhs)
		{
			this->set(rhs.g);
			int group_type = this->hed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		// passthru
		template <typename deduced>
		bool operator()(deduced &target)
		{
			if (top)
				(*this) = grup_top(top);
			while (this->under())
				if (next(*this)(target))
					return true;
			return false;
		}
		template <typename deduced>
		void at_any(deduced &temp)
		{
			// just calls the passthrough-operator()
			(*this)(temp);
		}
	};

	struct record : record_basic
	{
		record()
		{
		}
		record(iterator &i)
		{
			(*this) = i.next_record();
		}
		void operator=(const record_high &rhs)
		{
			this->set(rhs.r);
		}
		//bool operator()(record &target)
		//{
		//	// shouldnt compile
		//	return true;
		//}
	};

	template <typename next>
	struct closure : any
	{
		next last;
		void *pointer = nullptr;
		closure(void *pass) : pointer(pass)
		{
		}
		// passthru
		template <typename deduced>
		bool operator()(deduced &target)
		{
			while (it->under())
				if (next(*it)(target))
					return true;
			return false;
		}
		using any::any;
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
			two = rhs.two;
		}
		// passthru
		bool operator()(closure<record_and_grup> &arg)
		{
			if (one.editor_id((const char *)arg.pointer))
			{
				arg.last = *this;
				return true;
			}
			return false;
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