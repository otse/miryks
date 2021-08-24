#pragma once

#include <cstdarg>
#include <vector>

#include <lib.h>

#include <skyrim/skyrim.h>

// several useful things in here

namespace skyrim
{
	const char *getEditorIdOnly(const record *);

#define X Record

	class Record
	{
	public:
		const struct record *rcd;

		X()
		{
			rcd = nullptr;
		}
		X(const record *p)
		{
			rcd = p;
			if (rcd)
			{
				assertc(rcd->r == 'r');
				esp_check_rcd((record *)rcd);
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
		inline const subrecord *get(unsigned int i) const
		{
			return (const subrecord *)subrecords().elements[i];
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
		const subrecord* find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				const subrecord *sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			const subrecord *sub = find(sig, skip);
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
		inline bool hasEditorId(const char *name)
		{
			return 0 == strcmp(name, editorId());
		}
	};

#undef X

}