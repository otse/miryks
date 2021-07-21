#pragma once

// part of gloom

#include <dark/dark.h>

#include <dark/Ref.h>

#include <opengl/Types.h>
#include <opengl/Scene.h>
#include <opengl/Group.h>
#include <opengl/Geometry.h>
#include <opengl/Material.h>

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
} // namespace dark