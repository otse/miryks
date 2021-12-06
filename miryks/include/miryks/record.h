#pragma once

#include <cstdarg>
#include <vector>

#include <miryks/libs.h>

namespace miryks
{
	typedef unsigned int *formId;
	typedef const char *editorId;
	typedef const char signature[5];

	const char *getEditorIdOnly(const RECORD);
	
	struct record_basic
	{
		crecord *r = nullptr;

		record_basic()
		{
		}
		record_basic(crecord *r)
		{
			operator=(r);
		}
		void set(record_basic r)
		{
			operator=(r.r);
		}
		void operator=(crecord *rhs)
		{
			r = rhs;
			check();
		}
		void check()
		{
			if (!r)
				return;
			assertc(r->r == 'r');
			esp_check_rcd(r);
		}
		inline bool valid() const
		{
			return !!r;
		}
		inline const crecord_header &rhed() const
		{
			return *r->hed;
		}
		inline const revised_array &subrecords() const
		{
			return *r->rcdbs;
		}
		inline const SUBRECORD get(unsigned int i) const
		{
			return (const SUBRECORD)subrecords().elements[i];
		}
		inline bool is_type(signature sgn) const
		{
			return *(unsigned int *)sgn == rhed().sgn;
		}
		inline bool is_types(const std::vector<const char *> &sgns) const
		{
			for (const char *sgn : sgns)
				if (is_type(sgn))
					return true;
			return false;
		}
		const SUBRECORD find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				const SUBRECORD sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			const SUBRECORD sub = find(sig, skip);
			if (sub)
				return (T)sub->data;
			else
				return nullptr;
		}

		// << useful: >>

		inline editorId editor_id() const
		{
			return data<const char *>("EDID");
		}
		inline bool editor_id(const char *name) const
		{
			return editor_id() ? 0 == strcmp(name, editor_id()) : false;
		}
		inline const formId base() const
		{
			return data<unsigned int *>("NAME");
		}
		inline bool is_reference() const {
			return is_type("REFR");
		}
	};

}