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
		typedef cgrup *encapsulate;
		typedef esp_dud *low_any;
		typedef crecord *low_record;
		typedef cgrup *low_grup;
		encapsulate const ptr;
		int index;
		grup_iter(encapsulate cptr)
			: ptr(cptr)
		{
			index = 0;
			assertc(ptr->g == 'g');
			esp_check_grup(ptr);
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
		//inline 
		//inline bool correct() const
		//{
		//	return ptr;
		//}
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

	struct return_type
	{
		return_type() {}
	};

	/*template <typename next, int group_type>
	struct grup_base
	{
		grup_iter iter;
		grup_base(cgrup *cptr)
			: iter(cptr)
		{
		}
	};*/

	template <typename next,int group_type>
	struct grup_down
	{
		grup_iter iter;
		grup_down(grup_iter &iter)
			: grup_down(iter.next_grup()) {
		}
		grup_down(cgrup *cptr) : iter(cptr) {
		}
		bool call(void *result)
		{
			printf("grup_down call ptr %i\n", iter.ptr);
			assertc(iter.hed().group_type == group_type);
			while (iter.under())
			{
				if (next(iter).call(result))
					return true;
			}
			return false;
		}
	};

	/*template<
		typename capture_type,
		template<typename... Args> typename next,
		typename group_type
	>
	struct capture
	{
		grup_iter iter;
		capture(grup_iter::encapsulate iter)
			: iter(iter) {

		};
		capture_type call()
		{
			next<capture_type> temp;
			return capture_type;
			//return next(iter).call();
		}
	};*/

	struct recordt
	{
		typedef recordt base;
		crecord *ptr;
		recordt()
			: ptr(nullptr) {
		}
		recordt(crecord *p)
			: ptr(p) {
			printf("recordt\n");
			assertc(ptr->r == 'r');
			esp_check_rcd(ptr);
		}
		bool call(void *result)
		{
			printf("recordt call\n");
			return true;
		}
	};

	struct record_and_grup
	{
		typedef record_and_grup base;
		recordt one;
		cgrup *two;
		const char *id;
		bool failure;
		record_and_grup()
			: failure(true) {
			printf("failure\n");
		}
		record_and_grup(const char *id)
			:id(id) {
			printf("id\n");
		}
		record_and_grup(grup_iter &iter)
			: failure(false) {
			printf("record_and_grup\n");
			one = iter.next_record();
			two = iter.next_grup();
		}
		bool call(void *result)
		{
			printf("record_and_grup call\n");
			return true;
		}
	};

	//template<typename T = record_and_grup>
	//using capture_cell = grup_down<grup_down<grup_down<T, 3, T>, 2, T>, 0, T>;
}