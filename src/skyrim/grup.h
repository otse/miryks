#pragma once

#include <cstdarg>

#include <lib.h>

#include <skyrim/record.h>

#include <map>
#include <functional>

namespace skyrim
{
	class Record;

	class Grup
	{
	public:
		grupp grup;

		Grup()
		{
			(*this)(nullptr);
		}

		Grup(grupp grup)
		{
			assertc(grup);
			(*this)(grup);
		}

		Grup &operator()(grupp grup)
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

		void foreach(int group_type, std::function<bool(unsigned int &i)> f)
		{
			assertc(hed().group_type == group_type);
			for (unsigned int i = 0; i < amount(); i++)
				if (f(i))
					break;
		}
		
		template <typename T = void *>
		T get(unsigned int i, int type = -1) const
		{
			assertc(i < amount());
			assertc(i != -1 && this->type(i, type));
			return (T)grup->mixed.elements[i];
		}

		grupp getgrup(unsigned int i) const
		{
			return get<grupp>(i, GRUP);
		}

		recordp getrecord(unsigned int i) const
		{
			return get<recordp>(i, RECORD);
		}
	};
#undef X

	// Todo, CamelCase these types ??
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