#pragma once

#include <cstdarg>

#include <skyrim/record.h>

#include <map>
#include <array>
#include <functional>

// after any_constellation this was attempt two at templating the aging grup.h code

namespace skyrim
{
	struct grup_iter
	{
	public:
		typedef cgrup *encapsulate;
		typedef esp_dud *child_type;
		encapsulate ptr;
		int index;
		grup_iter(encapsulate cptr) : ptr(cptr)
		{
			index = 0;
			assertc(ptr->g == 'g');
			esp_check_grup(ptr);
			//printf("grupt label %.4s\n", (char *)&hed().label);
			//printf("grupt mixed %i\n", ptr->mixed);
		}
		inline bool valid() const
		{
			return ptr;
		}
		template <typename T>
		inline T get()
		{
			return (T)mixed().elements[index++];
		}
		inline const cgrup_header &hed() const
		{
			return *(ptr->hed);
		}
		inline const revised_array &mixed() const
		{
			return *(ptr->mixed);
		}
		inline bool under() const
		{
			return index < mixed().size;
		}
	};
	template <typename next, int group_type>
	class grup_base
	{
	public:
		grup_iter iter;
		grup_base(cgrup *cptr) : iter(cptr)
		{
		}
		bool call()
		{
			printf("grup_base call\n");
			return false;
		}
		inline next getcchild()
		{
			get<next::encapsulate>();
		}
	};

	template <typename next, int group_type>
	class grup_down: public grup_base<next, group_type>
	{
	public:
		typedef grup_base<next, group_type> base;
		typedef grup_down<next, group_type> self;
		grup_down(grup_iter &iter) : grup_down(iter.get<cgrup *>())
		{

		}
		grup_down(cgrup *cptr) : base(cptr) {
			printf("grup_down\n");
		}
		bool call()
		{
			printf("grup_down call ptr %i\n", this->iter.ptr);
			assertc(this->iter.valid());
			assertc(this->iter.hed().group_type == group_type);
			while (this->iter.under())
			{
				next child(this->iter);
				//if (child.call())
				//	return true;
			}
			return false;
		}
	};

	struct recordt
	{
		typedef recordt base;
		crecord *ptr = nullptr;
		recordt(grup_iter &iter) : recordt(iter.get<crecord *>()) {
		}
		recordt(crecord *cptr) : ptr(cptr) {
			printf("recordt\n");
			assertc(ptr->r == 'r');
			esp_check_rcd(ptr);
		}
		bool call()
		{
			printf("recordt call\n");
			return false;
		}
	};

	struct record_with_grup
	{

	};
}