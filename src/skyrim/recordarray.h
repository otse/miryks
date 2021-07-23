#pragma once

#include <lib.h>
#include <dark/dark.h>

#include <functional>

// using Objects = ObjectArray;

namespace dark
{

#define X ObjectArray
#define dud(i, type) type == get<Dud *>(i, type)->x

	class X
	{
	public:
		int filter = 0;
		bool stop;
		Grup *grup;
		X();
		X(Grup *);
		X &operator()(Grup *);

		unsigned int size() const;

		Grup *getgrup(unsigned int) const;
		Record *getrecord(unsigned int) const;

		void foreach(std::function<void(unsigned int &i)> f)
		{
			for (unsigned int i = 0; i < size(); i++)
			{
				if (filter == 0 || dud(i, filter))
					f(i);
				if (stop)
					break;
			}
		}

	protected:
		template <typename T = void *>
		T get(unsigned int i, int type = -1) const
		{
			assert((i < size() && i == -1) || dud(i, type));
			return (T)grup->mixed.elements[i];
		}
	};
#undef X
}
