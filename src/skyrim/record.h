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

	const char *getEditorIdOnly(const RCD);

#define X SKRecord

	class SKRecord
	{
	public:
		//const struct record *rcd;
		const RCD rcd;

		X()
		{
			rcd = nullptr;
		}
		X(const RCD p)
		{
			rcd = p;
			if (rcd)
			{
				assertc(rcd->r == 'r');
				esp_check_rcd((RCD)rcd);
			}
		}
		inline bool valid() const
		{
			return !!rcd;
		}
		inline const record_header &hed() const
		{
			return *rcd->hed;
		}
		inline const revised_array &subrecords() const
		{
			return *rcd->rcdbs;
		}
		inline const SRCD get(unsigned int i) const
		{
			return (const SRCD)subrecords().elements[i];
		}
		inline bool sig(signature sgn) const
		{
			return *(unsigned int *)sgn == hed().sgn;
		}
		inline bool sigany(const std::vector<const char *> &sgns) const
		{
			for (const char *sgn : sgns)
				if (sig(sgn))
					return true;
			return false;
		}
		const SRCD find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				const SRCD sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			const SRCD sub = find(sig, skip);
			if (sub)
				return (T)sub->data;
			else
				return nullptr;
		}
		inline editorId editorId() const
		{
			return data<const char *>("EDID");
		}
		inline formId base() const
		{
			return data<unsigned int *>("NAME");
		}

		// << useless >>
		inline bool HasId(const char *name)
		{
			return 0 == strcmp(name, editorId());
		}
	};

#undef X

}