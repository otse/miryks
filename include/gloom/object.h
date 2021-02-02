#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

#include <cstdarg>

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
	// see the example
	void gloom_objects_example(Record *);

	// an object wraps a record
	// has powerful getters that give you
	// access to its sub-records (and their important data!)

	class Object
	{
	public:
		// unsigned int formId;
		Record *const record;
		std::multimap<unsigned int, Subrecord *> fields;

		Object(Record *, int = 0);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;
		// bool AnyV(const char *fmt...) const;


		size_t Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		Subrecord *GetSubrecord(const char *, int) const;
		//Subrecord *GetFrom(unsigned int, int *) const;

		// Data Getter ! :
		template <typename T=void *>
		T Get(const char *type, int skip = 0) const
		{
			Subrecord *field = GetSubrecord(type, skip);
			return field ? (T)field->data : nullptr;
		}

	};

} // namespace gloom

#endif