#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{
	struct grup_wrapper
	{
		typedef esp_dud *low_any;
		typedef crecord *low_record;
		typedef cgrup *low_grup;
		typedef low_grup encapsulate;
		encapsulate g;
		int index;
		grup_wrapper(encapsulate e)
		{
			set(e);
		}
		void set(encapsulate e)
		{
			g = e;
			if (g)
			{
				index = 0;
				assertc(g->g == 'g');
				esp_check_grup(g);
			}
		}
		inline low_any get()
		{
			return (low_any)mixed().elements[index++];
		}
		inline low_record next_record()
		{
			return (low_record)get();
		}
		inline low_grup next_grup()
		{
			return (low_grup)get();
		}
		inline const cgrup_header &hed() const
		{
			return *(g->hed);
		}
		inline const revised_array &mixed() const
		{
			return *(g->mixed);
		}
		inline bool under() const
		{
			return index < mixed().size;
		}
	};

	struct any
	{
		any() {}
	};

	/*template <typename next, int group_type>
	struct grup_base
	{
		grup_wrapper iter;
		grup_base(cgrup *cptr)
			: iter(cptr)
		{
		}
	};*/

	template <typename next = any, int group_type = -1>
	struct grup : grup_wrapper
	{
		void operator=(const grup &rhs)
		{
			this->set(rhs.g);
		}
		grup() : grup_wrapper(nullptr)
		{
		}
		grup(grup_wrapper &w) : grup_wrapper(w.next_grup())
		{
		}
		grup(cgrup *cptr) : grup_wrapper(cptr)
		{
		}
		template <typename return_type>
		return_type til()
		{
			printf("grup til\n");
			assertc(this->hed().group_type == group_type);
			while (this->under())
			{
				if (return_type ret = next(*this).til<return_type>())
					return ret;
			}
			return return_type();
		}
	};

	struct record : record_wrapper
	{
		record() : record_wrapper()
		{
		}
		record(record &rhs) : record(rhs.r)
		{
		}
		record(crecord *p) : record_wrapper(p)
		{
			printf("recordt\n");
		}
		template <typename return_type>
		return_type til(void *needle)
		{
			printf("recordt call\n");
			return *this;
		}
	};

	struct match
	{
		bool good = true;
		match()
		{
			good = false;
		}
		operator bool() const
		{
			return good;
		}
	};

	struct record_and_grup : match
	{
		typedef record_and_grup base;
		record one;
		cgrup *two;
		const char *id;
		record_and_grup() : match()
		{
			// fail
		}
		record_and_grup(const char *id) : id(id)
		{
			printf("id %s\n", id);
		}
		record_and_grup(grup_wrapper &iter)
		{
			printf("record_and_grup\n");
			one = iter.next_record();
			two = iter.next_grup();
		}
		template <typename return_type>
		return_type til()
		{
			printf("record_and_grup call\n");
			if (one.editor_id("GloomGen"))
				return *this;
			return return_type();
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