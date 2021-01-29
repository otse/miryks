#include <libs>

#include <gloom/Object.h>

namespace gloom
{
	Object::Object(::Record *record, int n) : record(record)
	{
		if (record == nullptr)
			return;
		cassert(record->x == RECORD, "Object not record");
		int l = n ? n : record->fields.size;
		for (int i = 0; i < l; i++)
		{
			Subrecord *field = record->fields.subrecords[i];
			fields.emplace(field->hed->type, field);
		}
	}

	Subrecord *Object::GetField(const char *type, int skip) const
	{
		Subrecord *sub = nullptr;
		auto st = fields.equal_range(*(unsigned int *)type);
		for (auto it = st.first; it != st.second; ++it) {
			sub = it->second;
			if (skip-- <= 0)
				break;
		}
		return sub ? sub : nullptr;
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

	bool Object::Is(const char *type) const
	{
		return *(unsigned int *)type == record->hed->type;
	}

	bool Object::IsAny(std::vector<const char *> types) const
	{
		for (const char *type : types)
			if (*(unsigned int *)type == record->hed->type)
				return true;
		return false;
	}
} // namespace gloom