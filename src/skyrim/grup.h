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
		unsigned int index;
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

	
	struct match
	{
		grup_wrapper &w;
		match(grup_wrapper &w) : w(w)
		{
			printf("match\n");
		}
		template <typename return_type>
		bool operator()(return_type &temp) // deduce
		{
			printf("try match!\n");
			return_type capture(w);
			return capture(temp);
		}
		//operator bool() const
		//{
		//	return true;
		//}
	};

	struct any
	{
		any() {}
	};

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
		bool operator()(return_type &temp) // deduce
		{
			assertc(this->hed().group_type == group_type);
			while (this->under())
			{
				if (next(*this)(temp))
					return true;
			}
			return false;
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
		}
		template <typename return_type>
		bool operator()(return_type &temp) // deduce
		{
			printf("rare record til\n");
			return false;
		}
	};

	struct record_and_grup
	{
		typedef record_and_grup base;
		record one;
		grup<> two;
		const char *id;
		void operator=(record_and_grup &rhs)
		{
			one = rhs.one;
			two = rhs.two;
		}
		record_and_grup()
		{
		}
		record_and_grup(const char *id)
		{
			this->id = id;
			printf("id %s\n", id);
		}
		record_and_grup(grup_wrapper &w)
		{
			set(w);
		}
		void set(grup_wrapper &w)
		{
			one = w.next_record();
			two = w.next_grup();
		}
		template <typename return_type>
		bool operator()(return_type &temp) // deduce
		{
			if (one.editor_id(temp.id))
			{
				temp = *this;
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