#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(Grup *in)
	{
		ObjectArray objectArray(in);
		
		bool stop = false;
		
		// Default mode
		// Shows intent and filters types
		objectArray.ForEach(RECORD, stop, [&](ObjectArray &oa, size_t &i) {
			Object object(objectArray.GetRecord(i));
			stop = true;
		});

		// Mixed mode
		// Useful when you want the mixed unfiltered c-array
		objectArray.ForEach(0, stop, [&](ObjectArray &oa, size_t &i) {
			int type = objectArray.Type(i);
			i += 4;
		});
	}

	ObjectArray::ObjectArray(Grup *grup) : grup(grup)
	{
		cassert(grup, "objectarray grup null");
	}
} // namespace gloom