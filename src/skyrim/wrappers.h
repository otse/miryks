#pragma once

#include <cstdarg>

#include <lib.h>
//#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <map>
#include <functional>

namespace skyrim
{
	class RecordWrapper;
	class GrupWrapper;

	const char *getEditorIdOnly(const Record *);

	class RecordWrapper
	{
	public:
		Record *record;
		std::multimap<unsigned int, Subrecord *> subs;

		RecordWrapper()
		{
			record = nullptr;
		}

		RecordWrapper(Record *p)
		{
			set(p);
		};

		~RecordWrapper()
		{
			0;
		};

		const char *editorid()
		{
			return data<const char *>(_EDID_);
		}

		unsigned int *baseid()
		{
			return data<unsigned int *>(_NAME_);
		}

		void set(Record *p);

		Subrecord *finder(const char *type, int skip) const;

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
			Subrecord *field = finder(type, skip);
			return field ? (T)field->data : nullptr;
		}
	};

	class GrupWrapper
	{
	public:
		Grup *grup;

		GrupWrapper()
		{
			(*this)(nullptr); // calls operator !
		}

		GrupWrapper(Grup *grup)
		{
			assertc(grup);
			(*this)(grup);
		}

		GrupWrapper &operator()(Grup *grup)
		{
			this->grup = grup;
			return *this;
		}

		const grup_header &hed() const
		{
			return *grup->hed;
		}

		unsigned int amount() const
		{
			return grup->mixed.size;
		}

		bool type(unsigned int i, int type) const
		{
			return type == ((Dud *)grup->mixed.elements[i])->x;
		}

		void foreach (int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < amount(); i++)
				if (f(i))
					break;
		}

		Grup *getgrup(unsigned int i) const
		{
			return get<Grup *>(i, GRUP);
		}

		Record *getrecord(unsigned int i) const
		{
			return get<Record *>(i, RECORD);
		}

		RecordWrapper getrecordwrapper(unsigned int i) const
		{
			return RecordWrapper(getrecord(i));
		}

		GrupWrapper getgrupwrapper(unsigned int i) const
		{
			return GrupWrapper(getgrup(i));
		}

		template <typename T = void *>
		T get(unsigned int i, int type = -1) const
		{
			assertc(i < amount() && (i != -1 && this->type(i, type)));
			return (T)grup->mixed.elements[i];
		}
	};
#undef X

	enum GROUP_TYPES
	{
		TOP = 0,
		WORLD_CHILDREN,
		INTERIOR_CELL_BLOCK,
		INTERIOR_CELL_SUB_BLOCK,
		EXTERIOR_CELL_BLOCK,
		EXTERIOR_CELL_SUB_BLOCK,
		CELL_CHILDREN,
		TOPIC_CHILDREN,
		CELL_PERSISTENT_CHILDREN,
		CELL_TEMPORARY_CHILDREN
	};

}