#include <gloom/libs>

#include <gloom/Object.h>

namespace gloom
{
	Object::Object(::record *record) : record(record)
	{
		// printf("Object");

		NAME = nullptr;
		DATA = nullptr;
		EDID = nullptr;
		FULL = nullptr;
		DESC = nullptr;
		XSCL = nullptr;
		FNAM = nullptr;

		auto array = &record->fields;
		for (int i = 0; i < array->size; i++)
		{
			::subrecord *field = array->subrecords[i];
			fields.emplace(field->hed->type, field);
			if (field->hed->type == espwrd "EDID")
				EDID = ((char *)field->data);
			if (field->hed->type == espwrd "XSCL")
				XSCL = ((float *)field->data);
			if (field->hed->type == espwrd "NAME")
				NAME = ((unsigned int *)field->data);
			if (field->hed->type == espwrd "DATA")
				DATA = ((unsigned char *)field->data);
			if (field->hed->type == espwrd "FULL")
				FULL = ((char *)field->data);
			if (field->hed->type == espwrd "DESC")
				DESC = ((char *)field->data);
		}
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