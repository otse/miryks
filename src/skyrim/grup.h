#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

namespace skyrim
{
	constexpr static unsigned int cells = 1280066883;
	constexpr static unsigned int races = 1162035538;

	// grup passthru stuff

	// it works
	// but might seem pointless by an outsider

	template <unsigned int, typename>
	struct grup;
	struct grup_basic;
	struct grup_top;
	struct record;
	struct record_basic;
	struct record_check;

	template <typename>
	struct closure;
	struct record_and_grup;

	typedef grup_basic grup_implicit_downcast;
	typedef record_basic record_implicit_downcast;

	typedef grup_basic iterator;

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
		grup_top(
			int plugin, unsigned int top)
			: grup_top(plugin, (const char *)&top)
		{
			printf("grup_top uint %u const char %.4s\n", top, (const char *)&top);
		}
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
		any(
			iterator &i)
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

	typedef any here;

	template <unsigned int intended_group_type = 100, typename next = any>
	struct grup : grup_basic
	{
		unsigned int N = intended_group_type;
		typedef next T;
		grup(
			const grup &) = delete;
		grup(
			const char *top, int plugin)
		{
			operator=(grup_top(plugin, top));
		}
		grup(
			iterator &i)
		{
			operator=(i.next_grup());
		}
		grup(
			int plugin = 5)
		{
			if (N > 100)
				operator=(grup_top(plugin, N).g);
		}
		template <typename deduced>
		void operator/(
			deduced &rhs)
		{
			at_any(rhs);
		}
		void operator=(
			const grup_implicit_downcast &rhs)
		{
			this->set(rhs.g);
			int group_type = this->hed().group_type;
			assertc(
				N >= 100 ||
				N == group_type);
		}
		template <typename deduced>
		bool operator()(
			deduced &target)
		{
			while (this->under())
				if (T(*this)(target))
					return true;
			return false;
		}
		template <typename deduced>
		void at_any(deduced &temp)
		{
			(*this)(temp);
		}
	};

	struct record : record_basic
	{
		record()
		{
		}
		record(
			iterator &i)
		{
			(*this) = i.next_record();
		}
		void operator=(
			const record_implicit_downcast &rhs)
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
		closure(
			void *pass) : pointer(pass)
		{
		}
		auto &operator/(
			const char *id)
		{
			pointer = (void *)id;
			return *this;
		}
		template <typename deduced>
		bool operator()(
			deduced &target)
		{
			while (this->it->under())
			{
				T temp = T(*this->it);
				if (temp(target))
				{
					target.match = temp;
					return true;
				}
			}
			return false;
		}
		using any::any;
	};

	struct record_with_id : record
	{
		bool operator()(
			closure<record_with_id> &target)
		{
			return this->editor_id((const char *)target.pointer);
		}
	};

	struct record_and_grup
	{
		record one;
		grup<> two;
		record_and_grup()
		{
		}
		record_and_grup(
			iterator &i)
		{
			one = i.next_record();
			two = i.next_grup();
		}
		void operator=(
			const record_and_grup &rhs)
		{
			one = rhs.one;
			printf("try downcast on me\n");
			two = rhs.two;
		}
		// passthru
		bool operator()(
			closure<record_and_grup> &target)
		{
			return one.editor_id((const char *)target.pointer);
		}
	};

	namespace passthrough
	{

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
		CellTemporaryChildren,

		GrupTypeAnything = 100
	};
}