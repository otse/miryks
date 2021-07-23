#pragma once

#include <lib.h>
#include <dark/dark.h>

#include <functional>

// using Objects = ObjectArray;

namespace skyrim
{
	#define dud(i, type) type == get<Dud *>(i, type)->x

	#define X ObjectArray
	
	class ObjectArray
	{
	public:
		Grup *grup;
		bool stop;
		X();
		X(Grup *);
		X &operator()(Grup *); // useful!

		unsigned int amount() const;

		Grup *getgrup(unsigned int) const;
		Record *getrecord(unsigned int) const;
		Object getobject(unsigned int) const;
		ObjectArray getobjectarray(unsigned int) const;

		void foreach(std::function<void(unsigned int &i)> f)
		{
			for (unsigned int i = 0; i < amount(); i++)
			{
				f(i);
				if (stop)
					break;
			}
		}

		template <typename T = void *>
		T get(unsigned int i, int type = -1) const
		{
			assert(i < amount() && i == -1 || dud(i, type));
			return (T)grup->mixed.elements[i];
		}
	};
	#undef X

}
