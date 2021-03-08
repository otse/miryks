#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_test(Grup *grup)
	{
		ObjectArray objectArray(grup);
		
		bool stop = false;
		
		objectArray.Foreach(RECORD, stop, [&](Objects &objects, unsigned int &i) {
			Object object(objects.GetRecord(i));
		});

		using Objects = ObjectArray;

		Objects(grup).Foreach(0, stop, [&](Objects &objects, unsigned int &i) {
			i += 4;
			int type = objects.Type(i);
			stop = true;
		});
	}

	ObjectArray::ObjectArray(Grup *grup) : grup(grup)
	{
		cassert(grup, "objectarray grup null");
	}
} // namespace gloom