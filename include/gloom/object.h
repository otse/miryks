#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

#include <gloom/dark2.h>

#include <gloom/libs>

namespace gloom
{
	// high level record

	class Object
	{
	public:
		unsigned int formId;

		Record *const record;

		Object(::Record *, int = 0);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;

		std::multimap<unsigned int, Subrecord *> fields;

		size_t Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		template <typename T>
		T Get(const char *type, int skip = 0) const
		{
			Subrecord *field = GetField(type, skip);
			return field ? (T)field->data : nullptr;
		}

		Subrecord *GetField(const char *, int) const;
		Subrecord *GetFrom(unsigned int, int *) const;
	};

} // namespace gloom

#endif