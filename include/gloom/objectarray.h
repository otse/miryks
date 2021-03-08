#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

// part of gloom

#include <libs>
#include <Gloom/Gloom.h>

namespace gloom
{
	// using Objects = ObjectArray;

	class ObjectArray
	{
	public:
		int filter = 0;
		bool stop;
		Grup *grup;
		ObjectArray();
		ObjectArray(Grup *);
		ObjectArray &operator()(Grup *);
		unsigned int size() const
		{
			return grup->mixed.size;
		}
		void *get(unsigned int i) const
		{
			cassert(i < size(), "objects bounds");
			return grup->mixed.elements[i];
		}
		void *getSafe(unsigned int i, int type) const
		{
			cassert(type == getType(i), "objects type x");
			return get(i);
		}
		inline int getType(unsigned int i) const
		{
			return (int)((TypeDud *)get(i))->x;
		}
		Grup *getGrup(unsigned int i) const
		{
			return (Grup *)getSafe(i, GRUP);
		}
		Record *getRecord(unsigned int i) const
		{
			return (Record *)getSafe(i, RECORD);
		}
		template <class UnaryFunction>
		void forEach(UnaryFunction f)
		{
			for (unsigned int i = 0; i < size(); i++)
			{
				if (filter == 0 || getType(i) == filter)
					f(i);
				if (stop)
					break;
			}
		}

		// todo ressearch stl iter
		// https://gist.github.com/jeetsukumaran/307264
	};

} // namespace gloom

#endif