#pragma once

#include "dark2.h"

#include "ref.h"

#include "opengl/types.h"
#include "opengl/scene.h"
#include "opengl/group.h"
#include "opengl/geometry.h"
#include "opengl/material.h"

namespace dark2
{
	class Ref;

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
		Level(const char *);
		~Level();

		Cell loadedCell;

		static Cell GetCell(const char *);

		std::vector<Ref *> refs;
		std::map<std::string, Ref *> editorIds;

		void Update();
		void LoadCell(Cell &);
		void Unload();
		
		void ParseGrup(Cell &, Grup *);
	};
} // namespace dark2