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
		const recordp rp;

		Record()
		{
			rp = nullptr;
		}

		Record(recordp a)
		{
			rp = a;
		}

		subrecordp find(signature sgn, int skip = 0)
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				const subrecordp srp = get(i);
				if (*(unsigned int *)sgn == srp->hed->type)
					if (skip-- < 1)
						return srp;
			}
			return nullptr;
		}

		const record_header &hed() const
		{
			return *record->hed;
		}

		bool valid() const
		{
			return rp != nullptr;
		}

		const subrecordp get(unsigned int i) const
		{
			return rp->fields.subrecords[i];
		}

		unsigned int amount() const
		{
			return rp->fields.size;
		}

		bool oftype(signature sgn) const
		{
			return *(unsigned int *)sgn == rp->hed->type;
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
			subrecordp srp = find(type, skip);
			return srp ? (T)srp->data : nullptr;
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