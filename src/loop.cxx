#include <gloom/dark2.h>
#include <gloom/mesh.h>
#include <gloom/level.h>
#include <gloom/actor.h>
#include <libs>

#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/rt.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <gui/extra.h>

using namespace gloom;
using namespace glm;

GLFWwindow *window;

bool hideDebugGuis = true;
bool cursorShowing = false;
bool f10 = true;
bool h_pop = false;

namespace gloom
{
	ImFont *font2;
	ImFont *font3;

	void HideCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, Camera::prev[0], Camera::prev[1]);
		cursorShowing = false;
		first_person_camera->disabled = false;
	}

	void ShowCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorShowing = true;
		first_person_camera->disabled = true;
	}
} // namespace gloom

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	static bool F3 = false;

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		hideDebugGuis = !hideDebugGuis;
		if (!hideDebugGuis)
			ShowCursor();
		else
			HideCursor();
	}
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
		if (cursorShowing)
			HideCursor();
		else
			ShowCursor();
		camera->disabled = cursorShowing;
	}
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		printf("F5!\n");
		Esp *plugin = get_plugins()[1];
		const char *name = plugin->name;
		Esp *has = has_plugin(name);
		free_plugin(&has);
		get_plugins()[1] = loadEsp(name);
		delete dungeon;
		dungeon = new Level("GloomGen");
	}
	if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
	{
		SetShaderSources();
		for (auto &pair : Shader::shaders)
		{
			pair.second->Compile();
		}
	}
	if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
	{
		printf("f10");
		f10 = ! f10;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		h_pop = ! h_pop;
	}
}

static void doKeys()
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
	static double x2 = x;
	static double y2 = y;
	camera->Mouse((float)(x - x2), (float)(y - y2));
	x2 = x;
	y2 = y;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	gloom::width = width;
	gloom::height = height;
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
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer bindings
	const char *glsl_version = "#version 130";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void gloom::programGo()
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
		printf("glad");
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

void gloom::doImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	render_stats(&f10);

	if (h_pop)
		hero_menu();

	if (!hideDebugGuis)
	{
		bsa_gui();
		nifp_gui();
		esp_gui();
		cell_gui();
		opengl_gui();

		ImGui::ShowDemoWindow();
	}
}

void gloom::renderImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gloom::programLoop()
{
	render_target = new RenderTarget(width, height);
	RTQuad quad;

	double fps;
	int frames;
	double time, prevTime;
	char title[150];

	frames = 0;
	prevTime = glfwGetTime();

	scene->Add(first_person_camera->drawGroup);

	do
	{
		// from mrdoob stats
		time = glfwGetTime();
		if ((time - prevTime) > 1.0 || frames == 0)
		{
			fps = (double)frames / (time - prevTime);
			sprintf(title, "gloom %.0f fps - f1 for debug - f3 for mouse", fps);
			glfwSetWindowTitle(window, title);
			prevTime = time;
			frames = 0;
			gloom::fps = (int)fps;
		}
		frames++;

		//rt.Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		doImGui();

		doKeys();

		camera->Update(0.016f);

		// someDraugr
		if (someDraugr)
			someDraugr->Step();

		if (meanSkelly)
			meanSkelly->Step();

		if (someHuman)
			someHuman->Step();
		
		if (player1)
			player1->Step();

		dungeon->Update();

		scene->Order();
		scene->DrawItems();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		renderImGui();

		quad.geometry->Draw(mat4(1.0));

		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
