#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

// part of gloom

#include <libs>
#include <Gloom/Dark2.h>

namespace gloom
{
	using Objects = ObjectArray;

	void gloom_object_array_example(Grup *);

	class ObjectArray
	{
	public:
		Grup *const grup;
		ObjectArray(Grup *);
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
		void ForEach(int type, bool &stop, UnaryFunction f)
		{
			for (unsigned int i = 0; i < Size(); i++)
			{
				if (type == 0 || Type(i) == type)
					f(*this, i);
				if (stop)
					break;
			}
		}

		// todo ressearch stl iter
		// https://gist.github.com/jeetsukumaran/307264
	};

} // namespace gloom

#endif