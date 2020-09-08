#pragma once

#include "dark2.h"

#include "opengl/types"

namespace dark2
{
	class Reference
	{
	public:
		Reference();

		char *EDID = nullptr;
		float *DATA = nullptr;

		mat4 matrix;
		Mesh *mesh = nullptr;
		PointLight *pl = nullptr;

		void SetData(Record *);
	};
} // namespace dark2