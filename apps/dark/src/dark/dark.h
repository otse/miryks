#pragma once

#include <miryks/miryks.hpp>

#include <miryks/actors.h>

#include <opengl/renderer.h>

using namespace miryks;

void overlay_plugins();
void overlay_archives();
void overlay_models();
void overlay_cellview();
void overlay_rstats(bool *);

extern std::map<const char *, int> keys;

namespace dark
{
	void darkassert(bool);
	
	void view_bucket_in_place();
	void load_interior(const char *, bool);

	void init_dark();
	void loop();

	extern Monster *someDraugr;
}

void hero_menu();

void get_img();