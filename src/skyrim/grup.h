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
				index = 0;
				assertc(g->g == 'g'); // still blegh
				esp_check_grup(g);
			}
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
		bool under() const
		{
			return index < mixed().size;
		}
		const cgrup_header &hed() const { return *g->hed; }
		const revised_array &mixed() const { return *g->mixed; }
	};

	struct grup_top : grup_basic
	{
		static grup_top cells;
		grup_top(const char *top) : grup_basic(esp_top(get_plugins()[5], top))
		{
			printf("grup_top\n");
		}
	};

	struct any
	{
		grup_basic *const high;
		any() : high(nullptr)
		{
		}
		any(grup_basic &high) : high(&high)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			return deduced(*high)(target);
		}
	};

	template <typename next>
	struct closure : any
	{
		void *const pointer;
		closure(void *pointer) : pointer(pointer)
		{
		}
		closure(grup_basic &high) : any(high), pointer(nullptr)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			while (high->under())
				if (next(*high)(target))
					return true;
			return false;
		}
	};

	template <typename next = any, int intended_group_type = -1>
	struct grup : grup_basic
	{
		grup()
		{
		}
		// iterate
		grup(grup_high &high)
		{
			(*this) = high.next_grup();
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
		// iterate
		record(grup_high &high)
		{
			(*this) = high.next_record();
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

	struct record_and_grup
	{
		record bonnie;
		grup<> clyde;
		const char *id;
		record_and_grup(const char *id)
		{
			this->id = id;
		}
		record_and_grup(grup_high &high)
		{
			bonnie = high.next_record();
			clyde = high.next_grup();
		}
		bool operator()(record_and_grup &target)
		{
			if (bonnie.editor_id(target.id))
			{
				target.bonnie = bonnie;
				target.clyde = clyde;
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