#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	struct Cell
	{
		Record wrcd;
		Grup persistent, temporary;
	};

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		const char *editorId;
		bool alreadyTeleported = false;

		Cell loaded_cell;
		std::vector<Ref *> refs, iterables;
		std::map<std::string, Ref *> editorIds;

		void update();
		void parsegrup(int, Cell &, Grup);
		void loadcell();
		void unload();
		void placecamera();
	};

	static Cell capture_cell(Record wrcd, Grup wgrp)
	{
		Cell cell;
		cell.wrcd = wrcd;
		cell.persistent = wgrp.get<grup *>(0);
		cell.temporary = wgrp.get<grup *>(1);
		return cell;
	}

	// modern function that uses lambdas, with built-in group_type safety
	static Cell find_cell_foreach(const char *name)
	{
		Cell cell;
		Grup a, b, c;
		cgrupp top = esp_top_grup(get_plugins()[4], "CELL");
		bool stop = false;
		a(top).foreach(0, [&](unsigned int i) {
		b(a.get<grup *>(i)).foreach(2, [&](unsigned int j) {
		c(b.get<grup *>(j)).foreach(3, [&](unsigned int &k) {
			Record wrcd = c.get<record *>(k);
			Grup wgrp = c.get<grup *>(++k);
			if (wrcd.hasEditorId(name)) {
				cell = capture_cell(wrcd, wgrp);
				stop = true;
			}
			return stop;
			});
			return stop;
			});
			return stop;
		});
		return cell;
	}

	// the very same function but using for-loops
	// leave this here
	static Cell find_cell_loop(const char *name)
	{
		Cell cell;
		grupp top = esp_top_grup(get_plugins()[4], "CELL");
		Grup a, b, c;
		a = top;
		assertc(a.hed().group_type == 0);
		for (unsigned int i = 0; i < a.mixed().size; i++)
		{
			b = a.get<grup *>(i);
			assertc(b.hed().group_type == 2);
			for (unsigned int j = 0; j < b.mixed().size; j++)
			{
				c = b.get<grup *>(j);
				assertc(c.hed().group_type == 3);
				for (unsigned int k = 0; k < c.mixed().size; k++)
				{
					Record wrcd = c.get<record *>(k);
					Grup wgrp = c.get<grup *>(++k);
					if (wrcd.hasEditorId(name))
					{
						cell = capture_cell(wrcd, wgrp);
						goto endloop;
					}
				}
			}
		}
		endloop:
		return cell;
	}

	/*static void grup_dive_til(Grup wgrp, int group_type, editorId name)
	{
		printf("grup_dive_til");
		for (unsigned int i = 0; i < wgrp.mixed().size; i++)
		{
			if (wgrp.hed().group_type == group_type)
				return;
			else
				grup_dive_til(wgrp.get<grup *>(i), group_type, name);
		}
	}*/

}