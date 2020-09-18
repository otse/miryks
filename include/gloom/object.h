#ifndef GLOOM_BASEOBJECT_H
#define GLOOM_BASEOBJECT_H

#include <gloom/dark2.h>

namespace gloom
{
	// high level record

	class Object
	{
	public:
		::record *const record;

		Object(::record *, std::vector<const char *>);
		std::vector<const char *> search;

		unsigned int formId;

		unsigned int *NAME;
		unsigned char *DATA;
		char *EDID, *FULL, *DESC;
		float *XSCL, *FNAM;
	};

} // namespace gloom

#endif