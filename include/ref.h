#pragma once

#include <dark2.h>

#include <opengl/types.h>

namespace dark2
{
	struct REFR
	{
		REFR(record *);
		char *EDID = nullptr;
		float *XSCL = nullptr;
		unsigned int *NAME = nullptr;
		unsigned char *DATA = nullptr;
	};

	struct LIGH
	{
		LIGH(record *);
		char *EDID = nullptr;
		float *FNAM = nullptr;
		unsigned char *DATA = nullptr;
	};

	class Ref
	{
	public:
		Ref();
		~Ref();

		char *EDID = nullptr;
		float *DATA = nullptr;

		mat4 matrix;

		Mesh *mesh = nullptr;
		Renderable *renderable = nullptr;
		PointLight *pointlight = nullptr;

		void SetData(record *);
	};
} // namespace dark2