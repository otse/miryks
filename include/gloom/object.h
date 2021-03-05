#ifndef GLOOM_OBJECT_H
#define GLOOM_OBJECT_H

// part of gloom

#include <cstdarg>

#include <libs>
#include <Gloom/Gloom.h>

namespace gloom
{
	const char *GetEditorIdOnly(Record *);
	const char *GetEditorId(Object &);
	unsigned int *GetBaseId(Object &);

	// Field *GetField(Record *, unsigned int);

	void gloom_objects_test(Record *);

	class Object
	{
	public:
		Record *record = nullptr;
		Object(){};
		Object(Record *);
		~Object(){};
		void Set(Record *);
		bool Valid() { return record != nullptr; };
		std::multimap<unsigned int, Field *> fields;
		Field *EqualRange(const char *, int) const;
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
			Field *field = EqualRange(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};

} // namespace gloom

#endif