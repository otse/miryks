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
		grup_high next_grup()
		{
			return (grup_low)get_next();
		}
		record_high next_record()
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
		grup_wrapper *const iterator;
		any() : iterator(nullptr)
		{
		}
		any(grup_wrapper &iterator) : iterator(&iterator)
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			//printf("any operator()\n");
			if (std::is_same<deduced, any>::value)
			{
				printf("warn");
				return true;
			}
			else
				return deduced(*iterator)(target);
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
		void operator=(const grup_wrapper &iterator)
		{
			this->set(iterator.g);
			int group_type = this->hed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		grup()
		{
		}
		grup(grup_wrapper &iterator)
		{
			(*this) = iterator.next_grup();
		}
		grup(const char *word) : grup_wrapper(grup_top(word)) // :() !!
		{
		}
		template <typename deduced>
		bool operator()(deduced &target)
		{
			while (this->under())
			{
				if (next(*this)(target))
					return true;
			}
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
		record(record_low low) : record_wrapper(low)
		{
		}
		bool operator()(record &target)
		{
			// shouldnt compile
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
		record_and_grup(grup_wrapper &iterator)
		{
			bonnie = iterator.next_record();
			clyde = iterator.next_grup();
		}
		bool operator()(record_and_grup &target)
		{
			printf("checking id %s\n", bonnie.editor_id());
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