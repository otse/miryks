#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_test(Grup *grup)
	{
		using Objects = ObjectArray;

		ObjectArray objectArray(grup);
		objectArray.filter = RECORD;
		objectArray.Foreach([&](unsigned int &i) {
			Object object(objectArray.GetRecord(i));
		});

		Objects array;
		array(grup).Foreach([&](unsigned int &i) {
			i += 4;
			int type = array.Type(i);
			array.stop = true;
		});
	}

	ObjectArray::ObjectArray() {
		(*this)(nullptr);
	};

	ObjectArray::ObjectArray(Grup *grup)
	{
		cassert(grup, "objectarray grup null");
		(*this)(grup);
	}

	ObjectArray &ObjectArray::operator()(Grup *pass)
	{
		stop = false;
		grup = pass;
		return *this;
	}
} // namespace gloom