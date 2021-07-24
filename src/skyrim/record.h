#pragma once

#include <cstdarg>

#include <lib.h>
#include <dark/dark.h>
#include <skyrim/skyrim.h>

namespace skyrim
{
	const char *getEditorIdOnly(const Record *);
	const char *getEditorId(const Object &);
	unsigned int *getbaseid(const Object &);

	class Object
	{
	public:
		Record *record;
		std::multimap<unsigned int, Subrecord *> subs;
		Object();
		Object(Record *);
		~Object();
		void set(Record *);
		Subrecord *finder(const char *, int) const;
		bool valid()
		{
			return record != nullptr;
		};
		unsigned int count(const char *type) const
		{
			return subs.count(*(unsigned int *)type);
		}
		bool oftype(const char *type) const
		{
			return *(unsigned int *)type == record->hed->type;
		}
		bool oftypeany(std::vector<const char *> types) const
		{
			for (const char *type : types)
				if (oftype(type))
					return true;
			return false;
		}
		template <typename T = void *>
		T data(const char *type, int skip = 0) const
		{
			Subrecord *field = finder(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};

}