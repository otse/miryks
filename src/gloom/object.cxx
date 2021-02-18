#include <cstdarg>

#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	const char *GetEditorIdOnly(Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	const char *GetEditorId(Object &object)
	{
		return object.Data<const char *>("EDID", 0);
	}

	unsigned int *GetBaseId(Object &object)
	{
		return object.Data<unsigned int *>("NAME", 0);
	}

	//Subrecord *GetField(Record *record, unsigned int i)
	//{
	//	return (Subrecord *)record->fields.subrecords[i];
	//}

	void gloom_objects_example(Record *record)
	{
		Object object(record);
		if (object.IsType("REFR"))
		{
			auto formId = GetBaseId(object);
			auto editorId = GetEditorId(object);
			if (formId)
			{
				Object baseObject((Record *)formId);
				bool plant_or_weapon = baseObject.IsTypeAny({"FLOR", "WEAP"});
			}
			if (object.Count("MEOW") > 2)
				auto third = object.Data("MEOW", 2); // Get the third field
		}
	}

	Object::Object(Record *record) : record(record)
	{
		if (record == nullptr)
			return;
		cassert(((TypeDud *)record)->x == 2, "Gloom/Object Not Record");
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			Subrecord *field = record->fields.subrecords[i];
			fields.emplace(field->hed->type, field);
		}
	}

	Subrecord *Object::EqualRange(const char *type, int skip) const
	{
		Subrecord *sub = nullptr;
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

} // namespace gloom