#pragma once

#include "dark2.h"

#include "ref.h"

#include "opengl/types"
#include "opengl/scene"
#include "opengl/group"
#include "opengl/geometry"
#include "opengl/material"

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
		std::map<std::string, Reference *> editorIds;

		void Update();
		void LoadCell(Cell &);
		
		void ParseGrup(Cell &, Grup *);
	};
} // namespace dark2