#pragma once

#include "dark2.h"

extern "C"
{
#include "c/bsa.h"
#include "c/nif.h"
#include "c/esp.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

namespace dark2
{
	class Reference;

	struct Cell
	{
		bool good;
		Record *cell;
		Grup *persistent;
		Grup *non_persistent;
	};

	class Level
	{
	public:
		Level();

		//Cell cell;

		static Cell GetCell(const char *);

		std::vector<Reference *> references;

		void Update();
		void LoadCell(Cell &);
		
		void ParseGrup(Cell &, Grup *);
	};

	class Reference
	{
	public:
		Reference();

		mat4 matrix;

		void SetData(Record *);
	};
} // namespace dark2