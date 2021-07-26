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

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		crecordp rcd;

		Record()
		{
			rcd = nullptr;
		}

		Record(crecordp a)
		{
			rcd = a;
		}

		csubrecordp find(signature sgn, int skip = 0) const
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				csubrecordp sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}

		const record_header &hed() const
		{
			return *(rcd->hed);
		}

		bool valid() const
		{
			return rcd != nullptr;
		}

		csubrecordp get(unsigned int i) const
		{
			return rcd->fields.subrecords[i];
		}

		unsigned int amount() const
		{
			return rcd->fields.size;
		}

		bool oftype(signature sgn) const
		{
			return *(unsigned int *)sgn == rcd->hed->sgn;
		}

		bool oftypeany(std::vector<const char *> sigs) const
		{
			for (const char *sig : sigs)
				if (oftype(sig))
					return true;
			return false;
		}

		template <typename T = void *>
		T data(signature type, int skip = 0) const
		{
			csubrecordp sub = find(type, skip);
			return sub ? (T)sub->data : nullptr;
		}

		editorId editorId() const
		{
			return data<const char *>(EDID);
		}

		formId base() const
		{
			return data<unsigned int *>(NAME);
		}
	};
}