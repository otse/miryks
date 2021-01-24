#pragma once

#include <gloom/dark2.h>

#include <gloom/refr.h>

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
		bool good;
		Record *cell;
		Grup *persistent = nullptr;
		Grup *non_persistent = nullptr;
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
		
		void ParseGrup(Cell &, Grup *);
	};
} // namespace gloom