// todo this file is going

// merge with dark.cpp

#include <dark/dark.h>
#include <dark/window.h>

#include <miryks/player.h>
#include <miryks/model.h>
#include <miryks/grup.h>
#include <miryks/cell.h>
#include <miryks/trash.h>
#include <miryks/actors.h>

#include <panels.h>

#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/rendertarget.h>
#include <opengl/lights.h>

using namespace dark;
using namespace miryks;
using namespace glm;

ImFont *font2;
ImFont *font3;

static void hotswap_plugin_and_dungeon()
{
	reload_plugin();
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