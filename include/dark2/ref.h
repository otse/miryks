#pragma once

#include <dark2/dark2.h>

#include <opengl/types.h>

namespace dark2
{
	struct Refr
	{
		Refr() {};
		Refr(record *);
		char *EDID = nullptr;
		float *XSCL = nullptr;
		unsigned int *NAME = nullptr;
		unsigned char *DATA = nullptr;
	};

	struct Ligh
	{
		Ligh(record *);
		char *EDID = nullptr;
		float *FNAM = nullptr;
		unsigned char *DATA = nullptr;
	};

	struct Weap
	{
		Weap(record *);
		char *EDID = nullptr;
		char *FULL = nullptr;
		char *DESC = nullptr;
	};

	class Ref
	{
	public:
		Ref();
		~Ref();

		Refr REFR;
		Weap *WEAP = nullptr;
		record *baseObject = nullptr;

		char *EDID = nullptr;
		float *DATA = nullptr;

		mat4 matrix;

		Mesh *mesh = nullptr;
		Renderable *renderable = nullptr;
		PointLight *pointlight = nullptr;

		void SetData(record *);

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace dark2