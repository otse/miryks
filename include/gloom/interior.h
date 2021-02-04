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
		Record *cell = nullptr;
		Grup *persistent = nullptr;
		Grup *non_persistent = nullptr;
	};

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		Cell loadedCell;

		std::vector<Ref *> refs, iterables;

		std::map<std::string, Ref *> refEditorIds;

		static Cell GetCell(const char *);
		void Update();
		void LoadCell(Cell &);
		void Unload();
		
		void ParseGrup(Cell &, Grup *);
	};
} // namespace gloom