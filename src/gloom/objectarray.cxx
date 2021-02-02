#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(Grup *in)
	{
		using Objects = ObjectArray;

		ObjectArray objectArray(in);
		
		bool stop = false;
		
		// Filter mode
		objectArray.ForEach(RECORD, stop, [&](Objects &oa, size_t &i) {
			Object object(oa.GetRecord(i));
		});

		// Default mixed mode
		Objects(in).ForEach(0, stop, [&](Objects &oa, size_t &i) {
			i += 4;
			int type = oa.Type(i);
			stop = true;
		});
	}

	ObjectArray::ObjectArray(Grup *grup) : grup(grup)
	{
		cassert(grup, "objectarray grup null");
	}
} // namespace gloom