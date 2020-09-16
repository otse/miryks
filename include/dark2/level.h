#pragma once

#include <dark2/dark2.h>

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
		record *cell;
		grup *persistent = nullptr;
		grup *non_persistent = nullptr;
	};

	class Level
	{
	public:
		Level(const char *);
		~Level();

		Cell loadedCell;

		static Cell GetCell(const char *);

		std::vector<Ref *> refs, iterables;

		std::map<std::string, Ref *> refEditorIDs;

		void Update();
		void LoadCell(Cell &);
		void Unload();
		
		void ParseGrup(Cell &, grup *);
	};
} // namespace dark2