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

// several useful things in here

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
			if (p)
			{
				esp_read_lazy_record((recordp)rcd);
				assertc(rcd->r == 'r');
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
			return *rcd->subrecords;
		}
		inline csubrecordp get(unsigned int i) const
		{
			return (csubrecordp)subrecords().elements[i];
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
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			csubrecordp sub = find(sig, skip);
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