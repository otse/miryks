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

		Object(::record *);

		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;

		std::map<unsigned int, ::subrecord *> fields;

		unsigned int formId;
		
		unsigned int *NAME;
		unsigned char *DATA;
		char *EDID, *FULL, *DESC;
		float *XSCL, *FNAM;
	};

} // namespace gloom

#endif