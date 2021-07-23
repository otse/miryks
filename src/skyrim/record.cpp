#include <cstdarg>

#include <lib.h>

#include "record.h"
#include "recordarray.h"

namespace skyrim
{
	const char *getEditorIdOnly(Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	const char *getEditorId(Object &object)
	{
		return object.data<const char *>("EDID");
	}

	unsigned int *getBaseId(Object &object)
	{
		return object.data<unsigned int *>("NAME");
	}

	//Field *GetField(Record *record, unsigned int i)
	//{
	//	return (Field *)record->fields.subrecords[i];
	//}

	Object::Object(Record *record)
	{
		set(record);
	}

	void Object::set(Record *record) {
		fields.clear();
		this->record = record;
		if (record == nullptr)
			return;
		assert(((Dud *)record)->x == RECORD);
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			Field *field = record->fields.subrecords[i];
			fields.emplace(field->hed->type, field);
		}
	}

	Field *Object::equalRange(const char *type, int skip) const
	{
		Field *sub = nullptr;
		auto st = fields.equal_range(*(unsigned int *)type);
		for (auto it = st.first; it != st.second; ++it)
		{
			sub = it->second;
			if (skip-- <= 0)
				break;
		}
		assert(skip <= 0);
		return sub;
	}

	/*::subrecord *Object::GetFrom(unsigned int type, int *n) const
	{
		for (int i = *n; i < record->fields.size; i++)
		{
			::subrecord *field = record->fields.subrecords[i];
			if (type == field->hed->type)
			{
				*n = i;
				return field;
			}
		}
		return nullptr;
	}*/

} // namespace dark