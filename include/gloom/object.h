#ifndef GLOOM_BASEOBJECT_H
#define GLOOM_BASEOBJECT_H

#include <gloom/dark2.h>

#include <gloom/libs>

namespace gloom
{
	// high level record

	class Object
	{
	public:
		::record *const record;

		Object(::record *, int = 0);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;

		std::map<unsigned int, ::subrecord *> fields;

		unsigned int formId;

		template <typename T>
		T Get(const char *type) const
		{
			auto has = fields.find(*(unsigned int *)type);
			if (has == fields.end())
				return nullptr;
			return (T)has->second->data;
		};
	};

} // namespace gloom

#endif