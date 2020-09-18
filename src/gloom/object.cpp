#include <gloom/libs>

#include <gloom/Object.h>


namespace gloom
{
	Object::Object(::record *record, std::vector<const char *>) : record(record)
	{
		printf("Object");

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
			auto field = (array->subrecords[i]);
			if (field->hed->type == espwrd "EDID")
				EDID = ((char *)field->data);
			if (field->hed->type == espwrd "XSCL")
				XSCL = ((float *)field->data);
			if (field->hed->type == espwrd "NAME")
				NAME = ((unsigned int *)field->data);
			if (field->hed->type == espwrd "DATA")
				DATA = ((unsigned char *)field->data);
		}
	}
}