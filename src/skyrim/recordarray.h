#pragma once

#include <lib.h>
#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <functional>

namespace skyrim
{
	using Objects = ObjectArray;

#define X ObjectArray
	class ObjectArray
	{
	public:
		Grup *grup;
		X();
		X(Grup *);
		X &operator()(Grup *);

		const grup_header &hed() const;

		unsigned int amount() const;
		bool type(unsigned int, int) const;

		void foreach (std::function<bool(unsigned int &)>);

		Grup *getgrup(unsigned int i) const
		{
			return get<Grup *>(i, GRUP);
		}

		Record *getrecord(unsigned int i) const
		{
			return get<Record *>(i, RECORD);
		}

		Object getobject(unsigned int i) const
		{
			return Object(getrecord(i));
		}

		ObjectArray getobjectarray(unsigned int i) const
		{
			return ObjectArray(getgrup(i));
		}

		template <typename T = void *>
		T get(unsigned int i, int type = -1) const
		{
			assert(i < amount() && (i == -1 || type(i, type)));
			return (T)grup->mixed.elements[i];
		}
	};
#undef X

}