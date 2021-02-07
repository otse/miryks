#pragma once

#include <Gloom/Dark2.h>

#include <Gloom/Ref.h>

#include <opengl/types.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

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