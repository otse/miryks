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

	static Cell capture_cell(Record wrcd)
	{
		Cell cell;
		cell.wrcd = wrcd;
		Grup wgrp = (grupp)&wrcd+1;
		if (wgrp.mixed().size >= 1)
			cell.persistent = wgrp.getgrup(0);
		if (wgrp.mixed().size >= 2)
			cell.temporary = wgrp.getgrup(1);
		return cell;
	}

	static Cell find_cell_foreach(const char *name)
	{
		Cell cell;
		Grup A, B, C, D;
		cgrupp top = esp_top_grup(get_plugins()[3], "CELL");
		bool stop = false;
		A(top).foreach(0, [&](unsigned int i) {
		B(A.getgrup(i)).foreach(2, [&](unsigned int j) {
		C(B.getgrup(j)).foreach(3, [&](unsigned int &k) {
			Record wrcd = C.getrecord(k);
			Grup wgrp = C.getgrup(++k);
			if (wrcd.hasEditorId(name)) {
				cell = capture_cell(wrcd.rcd);
				stop = true;
			}
			return stop;
			});
			return stop;
			});
			return stop;
		});
	}

	const char *needle = 0;

	std::function< void(Grup) > *find_func = 0;

	static int grup_dive_til(Grup wgrp, int group_type, int ret = 0)
	{
		printf("woo");
		if (wgrp.hed().group_type == group_type)
			ret = *find_func(wgrp);
		if (ret)
			return;
		for (unsigned int i = 0; i < wgrp.mixed().size; i++)
			ret = grup_dive_til(wgrp.get<grup *>(i), group_type, ret);
	}

	static void find_cell(Grup haystack)
	{
		haystack.foreach(3, [&](unsigned int &i) {
			Record wrcd = haystack.getrecord(i);
			if (wrcd.hasEditorId(needle)) {
			printf("find_cell found !");
				Cell cell = capture_cell(wrcd);
				return true;
			}
			return false;
		});
	}
	
	static Cell find_cell_loop(const char *name)
	{
		Cell cell;
		grupp top = esp_top_grup(get_plugins()[3], "CELL");
		needle = name;
		find_func = find_cell;
		grup_dive_til(top, 3);
		Grup a, b, c;
		a = top;
		assertc(a.hed().group_type == 0);
		for (unsigned int i = 0; i < a.mixed().size; i++)
		{
			b = a.getgrup(i);
			assertc(b.hed().group_type == 2);
			for (unsigned int j = 0; j < b.mixed().size; j++)
			{
				c = b.getgrup(j);
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
}