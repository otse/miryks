#pragma once

#include <cstdarg>
#include <vector>

#include <miryks/libs.h>

// simple imperative access to subrecords and basic info
// for complicated mess look at grup.h

namespace miryks
{
	struct record_copy;
	typedef record_copy record_copy;

	typedef unsigned int *formId;
	typedef const char *editorId;
	typedef const char signature[5];

	const char *getEditorIdOnly(const RECORD);
	
	struct record_copy
	{
		typedef csubrecord *subrecord_type;
		crecord *r = nullptr;

		record_copy()
		{
		}
		record_copy(crecord *r)
		{
			setr(r);
		}
		void setr(crecord *p)
		{
			r = p;
			esp_check_rcd(r);
		}
		inline bool valid()
		{
			return !!r;
		}
		const crecord_header &rhed()
		{
			return *r->hed;
		}
		const revised_array &subrecords()
		{
			return *r->rcdbs;
		}
		subrecord_type get(unsigned int i)
		{
			return (subrecord_type)subrecords().elements[i];
		}
		bool is_type(signature sgn)
		{
			return *(unsigned int *)sgn == rhed().sgn;
		}
		bool is_types(const std::vector<const char *> &sgns)
		{
			for (const char *sgn : sgns)
				if (is_type(sgn))
					return true;
			return false;
		}
		subrecord_type find(signature sgn, int skip = 0)
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				subrecord_type sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		template <typename T = void *>
		T data(signature sig, int skip = 0)
		{
			subrecord_type sub = find(sig, skip);
			return sub ? (T)sub->data : nullptr;
		}

		// << useful: >>

		editorId editor_id()
		{
			return data<const char *>("EDID");
		}
		bool editor_id(const char *name)
		{
			return editor_id() ? 0 == strcmp(name, editor_id()) : false;
		}
		const formId base()
		{
			return data<unsigned int *>("NAME");
		}
		bool is_reference() {
			return is_type("REFR");
		}
	};

}