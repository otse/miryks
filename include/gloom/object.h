#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

#include <cstdarg>

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
	// util
	const char *GetEditorIdOnly(Record *);
	const char *GetEditorId(Object &);
	unsigned int *GetBaseId(Object &);

	// see the example
	void gloom_objects_example(Record *);

	// an object wraps a record
	// has powerful getters that give you
	// access to its sub-records (and their important data!)

	class Object
	{
	public:
		Record *const record = nullptr;
		std::multimap<unsigned int, Subrecord *> fields;

		Object(Record *);

		Subrecord *GetSubrecord(const char *, int) const;
		Subrecord *GetSubrecordIndex(unsigned int) const;

		bool Type(const char *type) const
		{
			return *(unsigned int *)type == record->hed->type;
		}

		bool TypeAny(std::vector<const char *> types) const
		{
			for (const char *type : types)
				if (*(unsigned int *)type == record->hed->type)
					return true;
			return false;
		}

		template <typename T = void *>
		T Data(const char *type, int skip = 0) const
		{
			Subrecord *field = GetSubrecord(type, skip);
			return field ? (T)field->data : nullptr;
		}

		unsigned int Object::Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		unsigned int Object::Size() const
		{
			return record->fields.size;
		}
	};

} // namespace gloom

#endif