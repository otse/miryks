#pragma once

#include <miryks/miryks.h>

#include <opengl/renderer.h>

using namespace miryks;

void overlay_plugins();
void overlay_archives();
void overlay_models();
void overlay_cellview();
void overlay_rstats(bool *);

namespace dark
{
	void darkassert(bool);

	void view_bucket_in_place();

	void init();
	void loop();
}

void hero_menu();

void get_img();