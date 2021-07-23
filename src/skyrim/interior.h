#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <renderer/types.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/material.h>

namespace dark
{
	class Ref;

	struct Cell
	{
		bool good = false;
		Record *record = nullptr;
		Grup *persistent = nullptr;
		Grup *non_persistent = nullptr;
	};

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		const char *editorId;

		bool alreadyTeleported = false;

		Cell loadedCell;

		std::vector<Ref *> refs, iterables;

		std::map<std::string, Ref *> editorIds;

		static Cell getCell(const char *);
		
		void update();
		void loadCell();
		void unload();
		void placeCamera();
		
		void parseGrup(Cell &, Grup *);
	};
}