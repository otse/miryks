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

		record *const record;

		Object(::record *, int = 0);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;

		std::multimap<unsigned int, subrecord *> fields;

		size_t Count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}

		template <typename T>
		T Get(const char *type, int skip = 0) const
		{
			subrecord *sub = nullptr;
			while (skip-- >= 0)
			{
				auto has = fields.find(*(unsigned int *)type);
				if (has != fields.end())
					sub = has->second;
			}
			return sub ? (T)sub->data : nullptr;
		};

		subrecord *Get2(const char *) const;
		subrecord *GetFrom(unsigned int, int *) const;
	};

} // namespace gloom

#endif