#pragma once

#include <miryks/miryks.hpp>

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

	void init();
	void loop();

	static inline bool pressing_key(const char *id)
	{
		return keys[id] == 1;
	}
	
	static inline bool holding_key(const char *id)
	{
		return keys[id] >= 1;
	}
}

void hero_menu();

void get_img();