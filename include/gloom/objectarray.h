#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
	// high level esp grup wrapper, see the example

	// remember a grup doesnt have subrecords

	void gloom_object_array_example(Grup *);

	class ObjectArray
	{
	public:
		Grup *const grup;
		//EspCArray *array;

		ObjectArray(Grup *);

		bool safe(size_t i, int type) const
		{
			return !type || ((TypeDud *)GetElement(i))->x == type;
		}

		size_t Length() const
		{
			return grup->mixed.size;
		}

		void *GetElement(size_t i) const
		{
			return grup->mixed.elements[i];
		}

		void *GetElementSafe(size_t i, int type) const
		{
			cassert(safe(i, type), "objectarray typex");
			return GetElement(i);
		}

		Grup *GetAsGrup(size_t i) const
		{
			return (Grup *)GetElementSafe(i, GRUP);
		}

		Record *GetAsRecord(size_t i) const
		{
			return (Record *)GetElementSafe(i, RECORD);
		}

		template <class UnaryFunction>
		void ForEach(int type, UnaryFunction f)
		{
			for (size_t i = 0; i < grup->mixed.size; i++)
			{
				void *t = GetElement(i);
				if (safe(i, type))
					f(t, i);
			}
		}

		// todo ressearch stl iter
		// https://gist.github.com/jeetsukumaran/307264
	};

} // namespace gloom

#endif