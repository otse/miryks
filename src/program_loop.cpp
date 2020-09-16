#include <dark2/dark2.h>
#include <dark2/mesh.h>
#include <dark2/level.h>
#include <dark2/libs>

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

using namespace dark2;
using namespace glm;

GLFWwindow *window;

bool hideDebugGuis = true;
bool cursorShowing = false;

namespace dark2
{
	ImFont *font2;

	void HideCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cursorShowing = false;
		first_person_camera->disabled = false;
	}

	void ShowCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorShowing = true;
		first_person_camera->disabled = true;
	}
} // namespace dark2

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
		if (cursorShowing) HideCursor(); else ShowCursor();
		camera->disabled = cursorShowing;
	}
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		printf("F5!\n");
		esp *plugin = get_plugins()[1];
		const char *name = plugin->name;
		esp *has = has_plugin(name);
		free_plugin(&has);
		get_plugins()[1] = loadPlugin(name);
		delete dungeon;
		dungeon = new Level("PadstowDungeon");
	}
	if (glfwGetKey(window, GLFW_KEY_F6))
	{
		SetShaderSources();
		for (auto &pair : Shader::shaders)
		{
			pair.second->Compile();
		}
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

	ImFont* font1 = io.Fonts->AddFontDefault();
	font2 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 45.0f);
	IM_ASSERT(font != NULL);

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	const char *glsl_version = "#version 130";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	
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

void dark2::doImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (!hideDebugGuis)
	{
		bsa_gui();
		nifp_gui();
		esp_gui();
		opengl_gui();

		ImGui::ShowDemoWindow();
	}
}

void dark2::renderImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dark2::programLoop()
{
	render_target = new RenderTarget(width, height);
	RTQuad quad;

	double fps;
	int frames;
	double time, prevTime;
	char title[150];

	frames = 0;
	prevTime = glfwGetTime();

	do
	{
		// from mrdoob stats
		time = glfwGetTime();
		if ((time - prevTime) > 1.0 || frames == 0)
		{
			fps = (double)frames / (time - prevTime);
			sprintf(title, "dark2 %.0f fps - f1 for debug - f3 for mouse", fps);
			glfwSetWindowTitle(window, title);
			prevTime = time;
			frames = 0;
		}
		frames++;

		//rt.Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		doImGui();

		doKeys();

		camera->Update(0.016);

		dungeon->Update();

		scene->Order();
		scene->DrawItems();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		renderImGui();

		quad.geometry->Draw(mat4(1.0));

		glfwSwapBuffers(window);
	}
	while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
