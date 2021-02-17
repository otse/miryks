#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

#include <cstdarg>

#include <libs>
#include <Gloom/Dark2.h>

// an object wraps a record
// has nifty interface to access fields (and their important data!)

namespace gloom
{
	const char *GetEditorIdOnly(Record *);
	const char *GetEditorId(Object &);
	unsigned int *GetBaseId(Object &);

	// Subrecord *GetField(Record *, unsigned int);

	void gloom_objects_example(Record *);

	class Object
	{
	public:
		Record *const record = nullptr;
		
		std::multimap<unsigned int, Subrecord *> fields;

		Object(Record *);

		Subrecord *EqualRange(const char *, int) const;

		unsigned int Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		bool IsType(const char *type) const
		{
			return *(unsigned int *)type == record->hed->type;
		}

		bool IsTypeAny(std::vector<const char *> types) const
		{
			for (const char *type : types)
				if (IsType(type))
					return true;
			return false;
		}

		template <typename T = void *>
		T Data(const char *type, int skip = 0) const
		{
			Subrecord *field = EqualRange(type, skip);
			return field ? (T)field->data : nullptr;
		}

	};

} // namespace gloom

#endif