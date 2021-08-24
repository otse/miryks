#include <dark/dark.h>



#include <skyrim/mesh.h>
#include <skyrim/grup.h>
#include <skyrim/interior.h>
#include <skyrim/trash.h>

#include <dark/actor.h>
#include <dark/collision.h>

#include <renderer/camera.h>
#include <renderer/scene.h>
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

bool hideDebugGuis = true;
bool cursorShowing = false;
bool f10 = false;
bool h_pop = false;
bool i_pop = false;

namespace dark
{
	ImFont *font2;
	ImFont *font3;

	void HideCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetCursorPos(window, Camera::prev[0], Camera::prev[1]);
		cursorShowing = false;
		Camera::DISABLE_LOOK = Camera::DISABLE_MOVEMENT = cursorShowing;
	}

	void ShowCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorShowing = true;
		Camera::DISABLE_LOOK = Camera::DISABLE_MOVEMENT = cursorShowing;
	}
} // namespace dark

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static bool useFbo = false;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	bool guing = ImGui::IsAnyItemActive();

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		hideDebugGuis = !hideDebugGuis;
		if (!hideDebugGuis)
			ShowCursor();
		else
			HideCursor();
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (cameraCur == viewerCam)
		{
			cameraCur = personCam;
			HideCursor();
		}
		else if (Cont::current)
		{
			Cont::Hide();
		}
		else
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	else if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		if (cursorShowing)
			HideCursor();
		else
			ShowCursor();
	}
	else if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
	{
		useFbo = !useFbo;
	}
	else if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		reload_my_plugin();
		reload_dungeon();
	}
	else if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
	{
		printf(" reload shaders ! \n");
		SetShaderSources();
		for (auto &pair : Shader::shaders)
		{
			pair.second->Compile();
		}
	}
	else if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
	{
		printf("f10");
		f10 = !f10;
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS && !guing)
	{
		h_pop = !h_pop;
	}
	else if (key == GLFW_KEY_I && action == GLFW_PRESS && !guing)
	{
		i_pop = !i_pop;
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS && !guing)
	{
		Refs::Activate();
	}
	else if (key == GLFW_KEY_V && action == GLFW_PRESS && !guing)
	{
		if (player1)
			player1->toggleView();
	}
}

static void doKeys()
{
	using namespace MyKeys;
	w = glfwGetKey(window, GLFW_KEY_W);
	a = glfwGetKey(window, GLFW_KEY_A);
	s = glfwGetKey(window, GLFW_KEY_S);
	d = glfwGetKey(window, GLFW_KEY_D);
	r = glfwGetKey(window, GLFW_KEY_R);
	f = glfwGetKey(window, GLFW_KEY_F);
	v = glfwGetKey(window, GLFW_KEY_F);
	shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
	static double x2 = x;
	static double y2 = y;
	cameraCur->Mouse((float)(x - x2), (float)(y - y2));
	x2 = x;
	y2 = y;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	printf("framebuffer_size_callback %i %i\n", width, height);
	glViewport(0, 0, width, height);
	::width = width;
	::height = height;
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
	font3 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 60.0f);
	IM_ASSERT(font2 != NULL);
	IM_ASSERT(font3 != NULL);
	

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

void put_it_fullscreen()
{
	printf("going fs !\n");
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = mode->width;
	height = mode->height;
	glfwSetWindowPos(window, 0, 0);
	glfwSetWindowSize(window, width, height);
	//delete render_target;
	//render_target = new RenderTarget(dark::width, dark::height, GL_RGB, GL_FLOAT);
	glViewport(0, 0, width, height);
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height, mode->refreshRate);
	//glfwSwapInterval(0); // vsync
}

void dark::setup_glfw()
{
	printf("setup window\n");

	glfwSetErrorCallback(glfw_error_callback);

	// todo blegh
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	window = glfwCreateWindow(width, height, "gloom", NULL, NULL);
	glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);

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
}

void dark::doImGui()
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
		nif_gui();
		esp_gui();
		cell_gui();
		opengl_gui();

		ImGui::ShowDemoWindow();
	}
}

void dark::renderImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dark::program_while()
{
	renderTargetDef = new RenderTarget(width, height, GL_RGB, GL_FLOAT);
	Quadt quad;

	double fps;
	int frames;
	double time, prevTime;
	char title[150];

	double now = 0, prev = 0;

	frames = 0;
	prevTime = glfwGetTime();

	sceneDef->drawGroups.Add(personCam->drawGroup);

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
			fps = (double)frames / (time - prevTime);
			sprintf(title, "gloom %.0f fps - f1 for debug - f3 for mouse", fps);
			glfwSetWindowTitle(window, title);
			prevTime = time;
			frames = 0;
			::fps = (int)fps;
		}
		frames++;

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

		glfwPollEvents();

		doImGui();

		//simple_loader();

		doKeys();

		if (player1)
			player1->step();

		cameraCur->Update(delta);

		#if 0
		myself->matrix = cameraCur->group->matrixWorld;
		myself->Calc();
		#endif
		
		// someDraugr
		if (someDraugr)
			someDraugr->step();

		if (meanSkelly)
			meanSkelly->step();

		if (someHuman)
			someHuman->step();

		if (dungeon)
			dungeon->update();

		Container::Step();
		//collision_simulate();

		//sceneDef->Order();
		sceneDef->DrawItems();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (useFbo)
		{
			quad.Draw(renderTargetDef);
		}

		renderImGui();

		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
