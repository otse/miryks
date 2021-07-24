#pragma once

#include <cstdarg>

#include <lib.h>
//#include <dark/dark.h>
#include <skyrim/skyrim.h>

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		recordp record = nullptr;

		Record(recordp p)
		{
			record = p;
			assertc(((Dud *)p)->x == RECORD);
		}

		~Record(){};

		const char *editorid()
		{
			return data<const char *>(_EDID_);
		}

		unsigned int *baseid()
		{
			return data<unsigned int *>(_NAME_);
		}

		subrecordp find(const char *type, int skip = s)
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				const subrecordp sub = get(i);
				if (*(unsigned int *)type == sub->hed->type)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}

		bool valid()` const
		{
			return record != nullptr;
		}

		const subrecordp get(unsigned int i) const
		{
			return record->fields.subrecords[i];
		}

		unsigned int amount() const
		{
			return record->fields.size();
		}

		bool oftype(const char *type) const
		{
			return *(unsigned int *)type == record->hed->type;
		}

		bool oftypeany(std::vector<const char *> types) const
		{
			for (const char *type : types)
				if (oftype(type))
					return true;
			return false;
		}

		template <typename T = void *>
		T data(const char *type, int skip = 0) const
		{
			subrecordp field = find(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};
}