#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{
	template <typename, int>
	struct grup;
	struct grup_basic;
	struct grup_top;
	struct record;
	struct record_basic;
	
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

	namespace passthrough
	{
		// pt et in here
	};

	struct any
	{
		iterator *it = nullptr;
		any()
		{
		}
		any(iterator &i) : it(&i)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			return deduced(*it)(target);
		}
	};

	template <typename next = any, int intended_group_type = -1>
	struct grup : grup_basic // inherit any somehow
	{
		grup()
		{
		}
		// iterate
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
		template <typename deduced>
		bool operator()(deduced &target)
		{
			while (this->under())
				if (next(*this)(target))
					return true;
			return false;
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
		void *pointer = nullptr;
		next last;
		closure(void *pass) : pointer(pass)
		{
		}
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

	struct record_and_grup : any
	{
		record one;
		grup<> two;
		void operator=(const record_and_grup &rhs)
		{
			// ugly manual copy
			one = rhs.one;
			two = rhs.two;
		}
		template<typename deduce>
		bool operator()(deduce &begin)
		{
			printf("target -> \n");
			return begin(*this);
		}
		bool operator()(closure<record_and_grup> &use_case)
		{
			one = it->next_record();
			two = it->next_grup();
			if (one.editor_id((const char *)use_case.pointer))
			{
				use_case.last = *this;
				return true;
			}
			return false;
		}
		using any::any;
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