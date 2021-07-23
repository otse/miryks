#pragma once

#include <cstdarg>

#include <lib.h>
#include <dark/dark.h>

namespace skyrim
{
	const char *getEditorIdOnly(::Record *);
	const char *getEditorId(Object &);
	unsigned int *getBaseId(Object &);

	// Field *GetField(Record *, unsigned int);

	class Object
	{
	public:
		::Record *record = nullptr;
		Object(){};
		Object(::Record *);
		~Object(){};
		void set(::Record *);
		bool valid() { return record != nullptr; };
		std::multimap<unsigned int, Field *> fields;
		Field *equalRange(const char *, int) const;
		unsigned int count(const char *type) const
		{
			return fields.count(*(unsigned int *)type);
		}
		bool isType(const char *type) const
		{
			return *(unsigned int *)type == record->hed->type;
		}
		bool isTypeAny(std::vector<const char *> types) const
		{
			for (const char *type : types)
				if (isType(type))
					return true;
			return false;
		}
		template <typename T = void *>
		T data(const char *type, int skip = 0) const
		{
			Field *field = equalRange(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};

}