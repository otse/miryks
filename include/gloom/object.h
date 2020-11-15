#ifndef GLOOM_BASEOBJECT_H
#define GLOOM_BASEOBJECT_H

#include <gloom/dark2.h>

#include <gloom/libs>

namespace gloom
{
	// high level record

	class Object
	{
	public:
		::record *const record;

		Object(::record *);

		template <typename T>
		T Get(const char *type) const
		{
			auto has = fields.find(espwrd type);
			if (has == fields.end())
				return nullptr;
			::subrecord *field = has->second;
			return (T)field->data;
		};
		void It(int);
		bool Is(const char *) const;
		bool IsAny(std::vector<const char *>) const;

		std::map<unsigned int, ::subrecord *> fields;

		unsigned int formId;
		/*
		unsigned int *NAME;
		unsigned char *DATA;
		char *EDID, *FULL, *DESC;
		float *XSCL, *FNAM;
		*/
	};

} // namespace gloom

#endif