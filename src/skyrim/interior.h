#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	struct CELL
	{
		bool good = false;
		crecordp record = nullptr;
		cgrupp persistent = nullptr;
		cgrupp temporary = nullptr;
	};

	struct Cell
	{
		Record rcd;
		Grup persistent, temporary;
	};

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		const char *editorId;
		bool alreadyTeleported = false;

		CELL loaded_cell;
		std::vector<Ref *> refs, iterables;
		std::map<std::string, Ref *> editorIds;

		static CELL getcell(const char *);
		void update();
		void parsegrup(int, CELL &, cgrupp);
		void loadcell();
		void unload();
		void placecamera();
	};

	static Cell capture_cell(Record wrcd, Grup wgrp)
	{
		Cell cell;
		if (wgrp.mixed().size >= 1)
			cell.persistent = wgrp.getgrup(0);
		if (wgrp.mixed().size >= 2)
			cell.temporary = wgrp.getgrup(1);
		return cell;
	}

	static void wgrup_dive_subblock(Grup wgrp, int t)
	{
		Grup wgrp;
		wgrp.foreach(t, [&](unsigned int i){ return false; });
	}

	static void find_cell(const char *name)
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
				cell = capture_cell(wrcd.rcd, wgrp.grp);
				stop = true;
			}
			return stop;
			});
			return stop;
			});
			return stop;
		});
	}
}