#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(Grup *grup)
	{
		ObjectArray objectArray(grup);
		
		bool stop = false;
		
		objectArray.ForEach(RECORD, stop, [&](Objects &oa, unsigned int &i) {
			Object object(oa.GetRecord(i));
		});

		using Objects = ObjectArray;

		Objects(grup).ForEach(0, stop, [&](Objects &oa, unsigned int &i) {
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