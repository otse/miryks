#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(EspCArray *array)
	{
		ObjectArray objectArray(array);

		// Default mode
		// Shows intent and safely filters types
		objectArray.ForEach(RECORD, [](void *element, size_t i) {
			Object object((Record *)element);

		});

		// Mixed mode
		// Useful when you want the mixed unfiltered carray
		objectArray.ForEach(0, [](void *element, size_t i) {
			int type = ((TypeDud *)element)->x;
		});
	}

	ObjectArray::ObjectArray(::EspCArray *array) : array(array)
	{
		cassert(array, "null esp array");
	}
} // namespace gloom