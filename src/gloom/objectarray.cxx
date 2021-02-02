#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(Grup *in)
	{
		ObjectArray objectArray(in);

		// Default mode
		// Shows intent and safely filters types
		objectArray.ForEach(RECORD, [](void *element, size_t i) {
			Object object((Record *)element);

		});

		// Mixed mode
		// Useful when you want the mixed unfiltered c-array
		objectArray.ForEach(0, [](void *element, size_t i) {
			// or simply use ObjectArray.safe(i, x)
			int type = ((TypeDud *)element)->x;
		});
	}

	ObjectArray::ObjectArray(Grup *grup) : grup(grup)
	{
		cassert(grup, "objectarray grup null");
	}
} // namespace gloom