#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(EspCArray *array)
	{
		ObjectArray objectArray(array);

		objectArray.ForEach(RECORD, [](void *element) {
			Object object((Record *)element);

		});

		objectArray.ForEach(0, [](void *element) {
			// unfiltered, useful for mixed cesparrays
		});
	}

	ObjectArray::ObjectArray(::EspCArray *array) : array(array)
	{
		cassert(array, "null esp array");
	}

	void object_array_example(EspCArray *espArray)
	{
		ObjectArray oArray = ObjectArray(espArray);
	}
} // namespace gloom