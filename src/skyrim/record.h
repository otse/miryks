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

#define inl inline

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

#define X Record

	// wrap for lib struct see /lib

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
		inl bool valid() const
		{
			return rcd != nullptr;
		}
		inl const record_header &hed() const
		{
			return *rcd->hed;
		}
		inl const revised_array &subs() const
		{
			return *rcd->subrecords;
		}
		inl const subrecord *get(unsigned int i) const
		{
			return (*(subrecord ***)&subs())[i];
		}
		const subrecord *find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < subs().size; i++)
			{
				const subrecord *sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		inl bool sig(signature sgn) const
		{
			return *(unsigned int *)sgn == rcd->hed->sgn;
		}
		inl bool sigany(const std::vector<const char *> &sgns) const
		{
			for (const char *sgn : sgns)
				if (sig(sgn))
					return true;
			return false;
		}
		template <typename T = void *>
		T data(signature sig, int skip = 0) const
		{
			const subrecord *sub = find(sig, skip);
			return sub ? (T)sub->data : nullptr;
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
		bool hasEditorId(const char *name) // maybe overload with editorId
		{
			return 0 == strcmp(name, editorId());
		}
	};
}