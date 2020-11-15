#include <gloom/libs>

#include <gloom/Object.h>

namespace gloom
{
	Object::Object(::record *record) : record(record)
	{
		// printf("Object");
		for (int i = 0; i < record->fields.size; i++)
		{
			::subrecord *field = record->fields.subrecords[i];
			fields.emplace(field->hed->type, field);
		}
	}
	
	void Object::It(int i)
	{
		auto array = &record->fields;
		::subrecord *field = array->subrecords[i];
		fields.emplace(field->hed->type, field);
	}

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