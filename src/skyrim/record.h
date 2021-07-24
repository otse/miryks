#pragma once

#include <cstdarg>

#include <lib.h>
//#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <map>
#include <functional>

namespace skyrim
{
	const char *getEditorIdOnly(const recordp);

	class Record
	{
	public:
		recordp record;

		// todo, get rid of map just use for loop

		std::multimap<unsigned int, subrecordp> subs;

		Record()
		{
			record = nullptr;
		}

		Record(recordp p)
		{
			set(p);
		}

		~Record() {};

		const char *editorid()
		{
			return data<const char *>(_EDID_);
		}

		unsigned int *baseid()
		{
			return data<unsigned int *>(_NAME_);
		}

		// todo rewrite
		void set(record_t *p)
		{
			record = p;
			subs.clear();
			if (record == nullptr)
				return;
			assertc(((Dud *)record)->x == RECORD);
			for (unsigned int i = 0; i < record->fields.size; i++)
			{
				subrecord_t *field = record->fields.subrecords[i];
				subs.emplace(field->hed->type, field);
			}
		}

		// todo rewrite
		subrecord_t *finder(const char *type, int skip) const
		{
			subrecord_t *sub = nullptr;
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

		bool valid() const
		{
			return record != nullptr;
		}

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
			subrecordp field = finder(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};
}