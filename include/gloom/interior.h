#pragma once

#include <Gloom/Dark2.h>

#include <Gloom/Ref.h>

#include <OpenGL/Types.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Group.h>
#include <OpenGL/Geometry.h>
#include <OpenGL/Material.h>

namespace gloom
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

		static Cell GetCell(const char *);
		
		void Update();
		void LoadCell();
		void Unload();
		void PlaceCamera();
		
		void ParseGrup(Cell &, Grup *);
	};
} // namespace gloom