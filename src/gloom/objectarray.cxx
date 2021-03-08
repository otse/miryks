#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
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