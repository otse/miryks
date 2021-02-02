#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

#include <cstdarg>

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
	// esp record wrapper that uses cool std::multimap, see the example

	void gloom_objects_example(Record *);

	class Object
	{
	public:
		// unsigned int formId;

		Record *const record;

		Object(::Record *, int = 0);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;
		// bool AnyV(const char *fmt...) const;

		std::multimap<unsigned int, Subrecord *> fields;

		size_t Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		// Data Getter !
		template <typename T=void *>
		T Get(const char *type, int skip = 0) const // Data Getter !
		{
			Subrecord *field = GetSubrecord(type, skip);
			return field ? (T)field->data : nullptr;
		}

		Subrecord *GetSubrecord(const char *, int) const;
		Subrecord *GetFrom(unsigned int, int *) const;
	};

} // namespace gloom

#endif