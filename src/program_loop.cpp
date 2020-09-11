#include "dark2.h"

extern "C"
{
#include "bsa.h"
#include "nif.h"
#include "esp.h"
}

#include "mesh.h"
#include "level.h"

#include "opengl/camera.h"
#include "opengl/scene.h"
#include "opengl/material.h"
#include "opengl/shader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace dark2;
using namespace glm;

GLFWwindow *window;

namespace dark2
{
	void HideCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void ShowCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
} // namespace dark2

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	static bool F3 = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
	{
		camera = first_person_camera;
		camera->disabled = false;
		HideCursor();
		F3 = false;
	}
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		F3 = !F3;
		F3 ? ShowCursor() : HideCursor();
		camera->disabled = F3;
	}
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		printf("F5!\n");
		const char *name = padstow->name;
		esp *plugin = has_plugin(name);
		free_plugin(&plugin);
		get_plugins()[1] = LoadPlugin(name);
		delete dungeon;
		dungeon = new Level("PadstowDungeon");
	}
	if (glfwGetKey(window, GLFW_KEY_F6))
	{
		ShaderSources::SetBufs();
	}
}

static void do_keys()
{
	if (!dynamic_cast<FirstPersonCamera *>(camera))
		return;
	first_person_camera->w = glfwGetKey(window, GLFW_KEY_W);
	first_person_camera->a = glfwGetKey(window, GLFW_KEY_A);
	first_person_camera->s = glfwGetKey(window, GLFW_KEY_S);
	first_person_camera->d = glfwGetKey(window, GLFW_KEY_D);
	first_person_camera->r = glfwGetKey(window, GLFW_KEY_R);
	first_person_camera->f = glfwGetKey(window, GLFW_KEY_F);
	first_person_camera->shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
	static double xx = x;
	static double yy = y;
	camera->Mouse(x - xx, yy - y);
	xx = x;
	yy = y;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	dark2::width = width;
	dark2::height = height;
}

void setupImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();
	//CustomStyle();

	// Setup Platform/Renderer bindings
	const char *glsl_version = "#version 130";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontDefault();
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void dark2::programGo()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Dork", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

#if 0
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#endif

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int er = gladLoadGL();
	if (er == 0)
	{
		log_("glad");
		exit(EXIT_FAILURE);
	}

	setupImgui();

	glClearColor(0, 0, 0, 1);

	glEnable(GL_TEXTURE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);
}

void dark2::programLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bsa_gui();
		nif_gui();
		esp_gui();
		//opengl_gui();
		ImGui::ShowDemoWindow();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		do_keys();

		camera->Update(0.016);

		dungeon->Update();

		scene->Order();
		scene->DrawItems();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
