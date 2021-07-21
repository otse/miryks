#include <cstdarg>

#include <libs>

#include <dark/Object.h>
#include <dark/ObjectArray.h>

namespace dark
{
	const char *getEditorIdOnly(Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	const char *getEditorId(Object &object)
	{
		return object.data<const char *>("EDID", 0);
	}

	unsigned int *getBaseId(Object &object)
	{
		return object.data<unsigned int *>("NAME", 0);
	}

	//Field *GetField(Record *record, unsigned int i)
	//{
	//	return (Field *)record->fields.subrecords[i];
	//}

	Object::Object(Record *record)
	{
		set(record);
	}

	void Object::set(Record *pass) {
		fields.clear();
		record = pass;
		if (record == nullptr)
			return;
		cassert(((TypeDud *)record)->x == 2, "Gloom/Object Not Record ??");
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
		cassert(skip <= 0, "field skip unsuccessful\n");
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