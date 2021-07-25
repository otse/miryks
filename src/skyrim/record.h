#pragma once

#include <cstdarg>

#include <lib.h>
#include <skyrim/skyrim.h>

namespace skyrim
{
	typedef unsigned int *formId;
	typedef const char *editorId;
	typedef const char signature[5];

	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		recordp record;

		Record()
		{
			record = nullptr;
		}

		Record(recordp p)
		{
			record = p;
		}

		subrecordp find(signature sig, int skip = 0)
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				const subrecordp sub = get(i);
				if (*(unsigned int *)sig == sub->hed->type)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}

		//const record_header &hed() const
		//{
		//	return *record->hed;
		//}

		bool valid() const
		{
			return !!record;
		}

		const subrecordp get(unsigned int i) const
		{
			return record->fields.subrecords[i];
		}

		unsigned int amount() const
		{
			return record->fields.size;
		}

		bool oftype(signature sig) const
		{
			return *(unsigned int *)sig == record->hed->type;
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
			subrecordp field = find(type, skip);
			return field ? (T)field->data : nullptr;
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