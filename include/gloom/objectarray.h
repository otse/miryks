#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
	using Objects = ObjectArray;

	// see the example
	void gloom_object_array_example(Grup *);

	// an objectarray is a wrapper for a grup

	// a grup doesnt handle subrecords
	// use Gloom/Object for that

	class ObjectArray
	{
	public:
		Grup *const grup;

		ObjectArray(Grup *);

		inline int Type(size_t i) const
		{
			return (int)((TypeDud *)Get(i))->x;
		}

		inline size_t Size() const
		{
			return grup->mixed.size;
		}

		void *Get(size_t i) const
		{
			cassert(i < Size(), "objectarray bounds");
			return grup->mixed.elements[i];
		}

		void *GetSafe(size_t i, int type) const
		{
			cassert(type == Type(i), "objectarray type x");
			return Get(i);
		}

		Grup *GetGrup(size_t i) const
		{
			return (Grup *)GetSafe(i, GRUP);
		}

		Record *GetRecord(size_t i) const
		{
			return (Record *)GetSafe(i, RECORD);
		}

		template <class UnaryFunction>
		void ForEach(int type, bool &stop, UnaryFunction f)
		{
			for (size_t i = 0; i < Size(); i++)
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