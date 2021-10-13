#pragma once

#include <cstdarg>
#include <vector>

#include <lib.h>

// several useful things in here

namespace skyrim
{
	typedef unsigned int *formId;
	typedef const char *editorId;
	typedef const char signature[5];

	const char *getEditorIdOnly(const RECORD);

	struct record
	{
		crecord *r;

		virtual void operator=(crecord *rhs)
		{
			r = rhs;
			check();
		}

		record()
		{
			r = nullptr;
		}
		record(crecord *r)
		{
			operator = (r);
		}
		void set(record r)
		{
			operator = (r.r);
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
		inline const crecord_header &hed() const
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
			return *(unsigned int *)sgn == hed().sgn;
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
		inline formId base() const
		{
			return data<unsigned int *>("NAME");
		}
	};

}