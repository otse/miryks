#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

// part of gloom

#include <libs>
#include <Gloom/Gloom.h>

namespace gloom
{
	// using Objects = ObjectArray;

	void gloom_object_array_test(Grup *);

	class ObjectArray
	{
	public:
		int filter = 0;
		bool stop;
		Grup *grup;
		ObjectArray();
		ObjectArray(Grup *);
		ObjectArray &operator()(Grup *);
		inline int Type(unsigned int i) const
		{
			return (int)((TypeDud *)_Get(i))->x;
		}
		unsigned int Size() const
		{
			return grup->mixed.size;
		}
		void *_Get(unsigned int i) const
		{
			cassert(i < Size(), "objects bounds");
			return grup->mixed.elements[i];
		}
		void *GetSafe(unsigned int i, int type) const
		{
			cassert(type == Type(i), "objects type x");
			return _Get(i);
		}
		Grup *GetGrup(unsigned int i) const
		{
			return (Grup *)GetSafe(i, GRUP);
		}
		Record *GetRecord(unsigned int i) const
		{
			return (Record *)GetSafe(i, RECORD);
		}
		template <class UnaryFunction>
		void Foreach(UnaryFunction f)
		{
			for (unsigned int i = 0; i < Size(); i++)
			{
				if (filter == 0 || Type(i) == filter)
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