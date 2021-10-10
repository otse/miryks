// todo this file is going

// merge with dark.cpp

#include <dark/dark.h>

#include <dark/player.h>

#include <skyrim/model.h>
#include <skyrim/grup.h>
#include <skyrim/cell.h>
#include <skyrim/trash.h>
#include <skyrim/actors.h>

#include <overlays/overlays.h>

#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/rendertarget.h>
#include <renderer/lights.h>

#include <window.h>

using namespace dark;
using namespace skyrim;
using namespace glm;

ImFont *font2;
ImFont *font3;

static void hotswap_plugin_and_dungeon()
{
	reload_my_esp();
	reload_dungeon();
}

static void reload_shaders()
{
	// warning glitchy
	printf(" reload shaders ! \n");
	SetShaderSources();
	for (auto &pair : Shader::shaders)
		pair.second->Compile();
}

static void toggle_third_person()
{
	if (player1)
		player1->toggleView();
}