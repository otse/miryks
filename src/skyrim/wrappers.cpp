#include "wrappers.h"

namespace skyrim
{

	const char *getEditorIdOnly(const Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	const char *getEditorId(const RecordWrapper &ob)
	{
		return ob.data<const char *>(_EDID_);
	}

	unsigned int *getBaseId(const RecordWrapper &ob)
	{
		return ob.data<unsigned int *>(_NAME_);
	}

	void RecordWrapper::set(Record *p)
	{
		record = p;
		subs.clear();
		if (record == nullptr)
			return;
		assertc(((Dud *)record)->x == RECORD);
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			Subrecord *field = record->fields.subrecords[i];
			subs.emplace(field->hed->type, field);
		}
	}

	Subrecord *RecordWrapper::finder(const char *type, int skip) const
	{
		Subrecord *sub = nullptr;
		auto ret = subs.equal_range(*(unsigned int *)type);
		for (auto it = ret.first; it != ret.second; ++it)
		{
			sub = it->second;
			if (skip-- <= 0)
				break;
		}
		assertc(skip <= 0);
		return sub;
	}
}