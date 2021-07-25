#pragma once

#include <cstdarg>

#include <lib.h>
#include <skyrim/skyrim.h>

#define REFR "REFR"
#define WEAP "WEAP"
#define MISC "MISC"
#define CELL "CELL"
#define EDID "EDID"
#define NAME "NAME"

namespace skyrim
{
	typedef unsigned int *formId;
	typedef const char *editorId;
	typedef const char signature[5];

	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		const recordp rcd;

		Record()
		{
			rcd = nullptr;
		}

		Record(recordp a)
		{
			rcd = a;
		}

		subrecordp find(signature sgn, int skip = 0)
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				const subrecordp srcd = get(i);
				if (*(unsigned int *)sgn == sbrcd->hed->type)
					if (skip-- < 1)
						return sbrcd;
			}
			return nullptr;
		}

		const record_header &hed() const
		{
			return *record->hed;
		}

		bool valid() const
		{
			return rcd != nullptr;
		}

		const subrecordp get(unsigned int i) const
		{
			return rcd->fields.subrecords[i];
		}

		unsigned int amount() const
		{
			return rcd->fields.size;
		}

		bool oftype(signature sgn) const
		{
			return *(unsigned int *)sgn == rcd->hed->type;
		}

		bool oftypeany(std::vector<signature> sigs) const
		{
			for (signature sig : sigs)
				if (oftype(sig))
					return true;
			return false;
		}

		template <typename T = void *>
		T data(signature type, int skip = 0) const
		{
			subrecordp sbrcd = find(type, skip);
			return sbrcd ? (T)sbrcd->data : nullptr;
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