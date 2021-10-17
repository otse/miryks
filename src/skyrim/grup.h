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
	struct grup_wrapper;
	struct record;
	struct record_wrapper;

	typedef cgrup *grup_low;
	typedef crecord *record_low;

	typedef grup_wrapper iterator; 

	struct grup_wrapper
	{
		typedef esp_dud *low_any;
		typedef grup_wrapper grup_high;
		typedef record_wrapper record_high;
		grup_low g;
		unsigned int index;
		grup_wrapper()
		{
			g = nullptr;
		}
		grup_wrapper(grup_low low)
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

	struct any
	{
		iterator *const gw;
		any() : gw(nullptr)
		{
		}
		any(iterator &gw) : gw(&gw)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			return deduced(*gw)(target);
		}
	};

	struct grup_top : grup_wrapper
	{
		static grup_top cells;
		grup_top(const char *top) : grup_wrapper(esp_top(get_plugins()[5], top))
		{
			printf("grup_top\n");
		}
	};

	template <typename next = any, int intended_group_type = -1>
	struct grup : grup_wrapper
	{
		void operator=(const iterator &gw)
		{
			this->set(gw.g);
			int group_type = this->hed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		grup()
		{
		}
		grup(iterator &gw)
		{
			(*this) = gw.next_grup();
		}
		grup(const char *word) : grup_wrapper(grup_top(word)) // :() !!
		{
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

	struct record : record_wrapper
	{
		// copy
		void operator=(const record_wrapper &rhs)
		{
			this->set(rhs.r);
		}
		record()
		{
		}
		record(iterator &gw) : record(gw.next_record())
		{
		}
		record(record_low low) : record_wrapper(low)
		{
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
		void *const value;
		closure(void *value) : value(value)
		{
		}
		closure(iterator &gw) : any(gw), value(nullptr)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			printf("closure next any ()\n");
			while (gw->under())
				if (next(*gw)(target))
					return true;
		}
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
		record_and_grup(iterator &gw)
		{
			bonnie = gw.next_record();
			clyde = gw.next_grup();
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