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

// this wraps a "lib struct"

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		const record *rcd;

		bool valid() const
		{
			return rcd != nullptr;
		}

		Record()
		{
			rcd = nullptr;
		}

		Record(const record *p)
		{
			rcd = p;
			assertc(rcd->r == 114);
		}

		const record_header &hed() const
		{
			return *(rcd->hed);
		}

		const subrecord *get(unsigned int i) const
		{
			return (*(subrecord ***)rcd->subrecords)[i];
		}

		const subrecord *find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < rcd->subrecords->size; i++)
			{
				const subrecord *sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}

		bool sig(signature sgn) const
		{
			return *(unsigned int *)sgn == rcd->hed->sgn;
		}

		bool sigany(const std::vector<const char *> &sgns) const
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