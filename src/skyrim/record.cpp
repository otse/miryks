#include <cstdarg>

#include <lib.h>

#include "record.h"
#include "recordarray.h"

namespace skyrim
{
	const char *getEditorIdOnly(const Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	const char *getEditorId(const Object &ob)
	{
		return ob.data<const char *>(_EDID_);
	}

	unsigned int *getbaseid(const Object &ob)
	{
		return ob.data<unsigned int *>(_NAME_);
	}

#define X Object

	X::X()
	{
		record = nullptr;
	}

	X::X(Record *re)
	{
		set(re);
	}

	X::~X()
	{
	}

	void X::set(Record *re)
	{
		record = re;
		subs.clear();
		if (record == nullptr)
			return;
		assert(((Dud *)record)->x == RECORD);
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			Field *field = record->fields.subrecords[i];
			subs.emplace(field->hed->type, field);
		}
	}

	Subrecord *X::finder(const char *type, int skip) const
	{
		Subrecord *sub = nullptr;
		auto st = subs.equal_range(*(unsigned int *)type);
		for (auto it = st.first; it != st.second; ++it)
		{
			sub = it->second;
			if (skip-- <= 0)
				break;
		}
		assert(skip <= 0);
		return sub;
	}

	/*::subrecord *Object::GetFrom(unsigned int type, int *n) const
	{
		for (int i = *n; i < record->subs.size; i++)
		{
			::subrecord *field = record->subs.subrecords[i];
			if (type == field->hed->type)
			{
				*n = i;
				return field;
			}
		}
		return nullptr;
	}*/

#undef X

} // namespace dark