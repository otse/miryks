#include <dark/dark.h>

#include <dark/player.h>

#include <skyrim/model.h>
#include <skyrim/grup.h>
#include <skyrim/interior.h>
#include <skyrim/trash.h>
#include <skyrim/actors.h>

#include <gooey/yagrum.h>

#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/rendertarget.h>
#include <renderer/lights.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <gooey/gooey.h>

using namespace dark;
using namespace skyrim;
using namespace glm;

GLFWwindow *window;

static bool hideDebugGuis = true;
static bool cursorShowing = false;
static bool f10 = false;
static bool useFbo = true;

namespace dark
{
ImFont *font2;
ImFont *font3;

void hide_cursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	cursorShowing = false;
}

void show_cursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cursorShowing = true;
}
}

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void toggle_debug()
{
	hideDebugGuis = !hideDebugGuis;
	if (!hideDebugGuis)
		show_cursor();
	else
		hide_cursor();
}

static void handle_esc()
{
	if (cameraCur == viewerCam)
	{
		cameraCur = personCam;
		hide_cursor();
	}
	else if (Cont::cur)
		Cont::Hide();
	else
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void toggle_fbo()
{
	useFbo = !useFbo;
}

static void toggle_cursor()
{
	if (cursorShowing)
		hide_cursor();
	else
		show_cursor();
}

static void hotswap_plugin_and_dungeon()
{
	reload_esp();
	reload_dungeon();
}

static void reload_shaders()
{
	// kind of glitchy
	printf(" reload shaders ! \n");
	SetShaderSources();
	for (auto &pair : Shader::shaders)
		pair.second->Compile();
}

static void toggle_render_stats()
{
	f10 = !f10;
}

static void toggle_third_person()
{
	if (player1)
		player1->toggleView();
}

static void handle_use_key()
{
	if (ImGui::IsAnyItemActive())
		return;
	Refs::Activate();
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

}

static void get_my_keys()
{
	// horribleness / shouldnt
	auto handler = [](const char *n, int k) -> void {
		int *p = &keys[n];
		int c = glfwGetKey(window, k);
		if (*p && c) *p = 2;
		else if (c) *p = 1;
		else if (!c) *p = 0;
	};
	handler("esc", GLFW_KEY_ESCAPE);
	handler("f1", GLFW_KEY_F1);
	handler("f2", GLFW_KEY_F2);
	handler("f3", GLFW_KEY_F3);
	handler("f4", GLFW_KEY_F4);
	handler("f5", GLFW_KEY_F5);
	handler("f6", GLFW_KEY_F6);
	handler("f10", GLFW_KEY_F10);
	handler("w", GLFW_KEY_W);
	handler("a", GLFW_KEY_A);
	handler("s", GLFW_KEY_S);
	handler("d", GLFW_KEY_D);
	handler("r", GLFW_KEY_R);
	handler("f", GLFW_KEY_F);
	handler("v", GLFW_KEY_V);
	handler("e", GLFW_KEY_E);
	handler("lctrl", GLFW_KEY_LEFT_CONTROL);
	handler("lshift", GLFW_KEY_LEFT_SHIFT);
}

static void handle_most_keys()
{
	if (pressing("esc")) handle_esc();
	else if (pressing("f1")) toggle_debug();
	else if (pressing("f2")) yagrum_queue("", 5, true);
	else if (pressing("f3")) toggle_cursor();
	else if (pressing("f4")) toggle_fbo();
	else if (pressing("f5")) hotswap_plugin_and_dungeon();
	else if (pressing("f6")) reload_shaders();
	else if (pressing("f10")) toggle_render_stats();
	else if (pressing("e")) handle_use_key();
	else if (pressing("v")) toggle_third_person();
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
	static double x2 = x;
	static double y2 = y;
	if (!cursorShowing)
		cameraCur->Mouse((float)(x - x2), (float)(y - y2));
	x2 = x;
	y2 = y;
}

void setupImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImFont *font1 = io.Fonts->AddFontDefault();
	font2 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 45.0f);
	font3 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 55.0f);
	IM_ASSERT(font2 != NULL);
	IM_ASSERT(font3 != NULL);
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void put_it_fullscreen()
{
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = mode->width;
	height = mode->height;
	glfwSetWindowPos(window, 0, 0);
	glfwSetWindowSize(window, width, height);
	glViewport(0, 0, width, height);
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height, mode->refreshRate);
	glfwSwapInterval(1); // vsync
	// glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
}

void dark::goingrate()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	window = glfwCreateWindow(width, height, "dark", NULL, NULL);

	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, cursor_pos_callback);

	hide_cursor();

	if (glfwRawMouseMotionSupported())
    	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	int er = gladLoadGL();
	if (er == 0)
	{
		printf("glad");
		exit(EXIT_FAILURE);
	}

	setupImgui();

	glClearColor(0, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void imgui_calls()
{
	render_stats(&f10);

	yagrum_checker();

	if (!hideDebugGuis)
	{
		overlay_archives();
		overlay_models();
		overlay_plugins();
		cell_gui();
		opengl_gui();

		ImGui::ShowDemoWindow();
	}
}

void dark::program_while()
{
	renderTargetDef = new RenderTarget(width, height, GL_RGB, GL_FLOAT);
	Quadt quad;

	double fps_;
	int frames;
	double time, prevTime;
	char title[150];

	double now = 0, prev = 0;

	frames = 0;
	prevTime = glfwGetTime();

	sceneDef->bigGroup->Add(personCam->drawGroup);

	#if 0
	PointLight *myself = new PointLight;
	myself->color = vec3(1.f);
	myself->distance = 500.0f;

	sceneDef->pointLights.Add(myself);
	#endif
	
	do
	{
		time = glfwGetTime();
		now = time;
		delta = now - prev;
		prev = now;
		if ((time - prevTime) > 1.0 || frames == 0)
		{
			fps_ = (double)frames / (time - prevTime);
			prevTime = time;
			frames = 0;
			fps = (int)fps_;
		}
		frames++;

		Group::drawCalls = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		if (useFbo)
		{
			renderTargetDef->Bind();
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();

		//simple_loader();

		get_my_keys();

		handle_most_keys();

		cameraCur->Update(delta);
		
		if (player1)
			player1->Step();

		#if 0
		myself->matrix = cameraCur->group->matrixWorld;
		myself->Calc();
		#endif
		
		// someDraugr
		if (someDraugr)
			someDraugr->Step();

		if (meanSkelly)
			meanSkelly->Step();

		if (someHuman)
			someHuman->Step();

		if (dungeon)
			dungeon->Update();

		Container::Step();

		sceneDef->DrawItems();

		imgui_calls();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (useFbo)
		{
			quad.Draw(renderTargetDef);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
