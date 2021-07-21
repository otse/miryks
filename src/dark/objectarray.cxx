#include <libs>

#include <dark/Object.h>
#include <dark/ObjectArray.h>

namespace dark
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
} // namespace dark