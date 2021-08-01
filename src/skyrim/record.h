#pragma once

#include <cstdarg>
#include <vector>

#include <lib.h>
#include <skyrim/skyrim.h>

#define REFR "REFR"
#define WEAP "WEAP"
#define MISC "MISC"
#define EDID "EDID"
#define NAME "NAME"

// wrap for lib struct see /lib

// instead of a for loop, you use foreach
// the raw getter is

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

#define X Record

	class Record
	{
	public:
		crecordp rcd;

		X()
		{
			rcd = nullptr;
		}
		X(crecordp p)
		{
			rcd = p;
			assertc(rcd->r == 'r');
		}
		inline bool valid() const
		{
			return rcd != nullptr;
		}
		inline const record_header &hed() const
		{
			return *rcd->hed;
		}
		inline const revised_array &subrecords() const
		{
			return *rcd->subrecords;
		}
		inline csubrecordp get(unsigned int i) const
		{
			return (csubrecordp)subrecords().elements[i];
		}
		csubrecordp find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				csubrecordp sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
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
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			csubrecordp sub = find(sig, skip);
			if (sub)
				return (T)sub->data;
			else
				return nullptr;
		}
		editorId editorId() const
		{
			return data<const char *>("EDID");
		}
		formId base() const
		{
			return data<unsigned int *>("NAME");
		}

		// << useless >>
		bool hasEditorId(const char *name)
		{
			return 0 == strcmp(name, editorId());
		}
	};

#undef X

}