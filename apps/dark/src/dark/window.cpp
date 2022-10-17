#include <dark/dark.h>
#include <dark/window.h>
#include <dark/trash.h>
#include <dark/my_reference.h>

#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/rendertarget.h>
#include <opengl/scene_graph.hpp>

#include <opengl/shader.h>

#include <miryks/player.h>
#include <miryks/collision.h>

#include <panels.h>

GLFWwindow *window = nullptr;

static bool cursorShowing = false;
static bool hideOverlays = true;
static bool f10 = false;
static bool useFbo = false;
static bool wireframe = false;

void setupImgui();

static void toggle_cursor()
{
	cursorShowing = !cursorShowing;
}

static void errorCallback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void cursorPosCallback(GLFWwindow *window, double x, double y)
{
	static double x2 = x;
	static double y2 = y;
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		cameraCur->Mouse((float)(x - x2), (float)(y - y2));
	x2 = x;
	y2 = y;
}

void dark::init_dark()
{
	// miryks::hooks::some_behavior = [](int a) { return false; };

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vidMode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	width = vidMode->width;
	height = vidMode->height;

	window = glfwCreateWindow(width, height, "dark", NULL, NULL);

	glfwSetWindowPos(window, 0, 0);

	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, cursorPosCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSwapInterval(1);

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

static bool show_hero_menu = false;

void imgui_calls()
{
	overlay_rstats(&f10);

	if (show_hero_menu)
		hero_menu();

	yagrum_checker();

	if (!hideOverlays)
	{
		overlay_archives();
		overlay_models();
		overlay_plugins();
		overlay_cellview();
		overlay_renderer();

		ImGui::ShowDemoWindow();
	}
}

void windowed()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vidMode = glfwGetVideoMode(monitor);

	if (window)
		glfwDestroyWindow(window);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "dark", NULL, NULL);

	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, cursorPosCallback);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowPos(
		window,
		vidMode->width / 2 - width / 2,
		vidMode->height / 2 - height / 2);

	glfwSwapInterval(1);
}

void borderless()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vidMode = glfwGetVideoMode(monitor);

	if (window)
		glfwDestroyWindow(window);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	width = vidMode->width;
	height = vidMode->height;

	window = glfwCreateWindow(width, height, "dark", NULL, NULL);

	glfwSetWindowPos(window, 0, 0);

	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, cursorPosCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSwapInterval(1);
}

static void toggle_debug()
{
	hideOverlays = !hideOverlays;
	if (!hideOverlays)
		cursorShowing = true;
	else
		cursorShowing = false;
}

static void toggle_fbo()
{
	useFbo = !useFbo;
}

static void handle_esc()
{
	if (cameraCur == viewerCam)
	{
		cameraCur = personCam;
		cursorShowing = false;
		yagrum_pop();
		yagrum_queue("", 2, true);
	}
	// else if (Cont::cur)
	//	Cont::Hide();
	else
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void toggle_windowed()
{
	// static bool fs = true;
	// fs = !fs;
	// if (fs)
	//{
	//	borderless();
	// }
	// else
	{
		width = 1024;
		height = 768;
		windowed();
	}
}

static void capture_keys()
{
	// horribleness
	auto handler = [](int k, const char *n) -> void
	{
		int *p = &keys[n];
		int c = glfwGetKey(window, k);
		if (*p && c)
			*p = 2;
		else if (c)
			*p = 1;
		else if (!c)
			*p = 0;
	};
	auto list = [&](const std::map<int, const char *> &pairs)
	{
		for (auto pair : pairs)
			handler(pair.first, pair.second);
	};
	list({{GLFW_KEY_ESCAPE, "esc"},
		  {GLFW_KEY_F1, "f1"},
		  {GLFW_KEY_F2, "f2"},
		  {GLFW_KEY_F3, "f3"},
		  {GLFW_KEY_F4, "f4"},
		  {GLFW_KEY_F5, "f5"},
		  {GLFW_KEY_F6, "f6"},
		  {GLFW_KEY_F7, "f7"},
		  {GLFW_KEY_F8, "f8"},
		  {GLFW_KEY_F9, "f9"},
		  {GLFW_KEY_F10, "f10"},
		  {GLFW_KEY_SPACE, "space"},
		  {GLFW_KEY_W, "w"},
		  {GLFW_KEY_A, "a"},
		  {GLFW_KEY_S, "s"},
		  {GLFW_KEY_D, "d"},
		  {GLFW_KEY_R, "r"},
		  {GLFW_KEY_F, "f"},
		  {GLFW_KEY_V, "v"},
		  {GLFW_KEY_E, "e"},
		  {GLFW_KEY_H, "h"},
		  {GLFW_KEY_LEFT_CONTROL, "lctrl"},
		  {GLFW_KEY_LEFT_SHIFT, "lshift"},
		  {GLFW_KEY_LEFT_ALT, "lalt"},
		  {GLFW_KEY_ENTER, "enter"}});
}

void setupImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImFont *font1 = io.Fonts->AddFontDefault();
	font2 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 45.0f);
	font3 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 55.0f);
	IM_ASSERT(font2 != NULL);
	IM_ASSERT(font3 != NULL);
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

static void handle_use_key()
{
	if (ImGui::IsAnyItemActive())
		return;
	dark::itemfinder::activate();
}

static void reload_shaders()
{
	// warning glitchy
	printf(" reload shaders ! \n");
	SetShaderSources();
	for (auto &pair : Shader::shaders)
		pair.second->Compile();
}

static void hotswap_plugin_and_dungeon()
{
	printf("hotswap plugin and dungeon\n");
	free_plugin(&get_plugins()[5]);
	get_plugins()[5] = load_plugin(PLUGIN_0, true);
	const char *id = ginterior->id;
	hooks::load_interior(id, 5, false);
}

static void handle_my_keys()
{
	using namespace miryks;
	capture_keys();
	if (pressing_key("esc"))
		handle_esc();
	else if (pressing_key("f1"))
		toggle_debug();
	else if (pressing_key("f2"))
		yagrum_queue("", 2, true);
	else if (pressing_key("f3"))
		toggle_cursor();
	else if (pressing_key("f4"))
		toggle_fbo();
	else if (pressing_key("f5"))
		hotswap_plugin_and_dungeon();
	else if (pressing_key("f6"))
		reload_shaders();
	else if (pressing_key("f7"))
	{
		wireframe = !wireframe;
		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	else if (pressing_key("f8")) {
		if (ginterior)
		{
			delete ginterior;
			ginterior = nullptr;
		}
		if (!gworldspace)
		{
			hooks::load_world_space("DarkWorld", 5, true); // Blackreach
		}
	}
	else if (pressing_key("f10"))
		f10 = !f10;
	else if (pressing_key("e"))
		handle_use_key();
	else if (pressing_key("h"))
	{
		printf("show_hero_menu\n");
		show_hero_menu = !show_hero_menu;
	}
	// else if (pressing_key("v"))
	// third_person();
	else if (holding_key("lalt") && pressing_key("enter"))
	{
		printf("going windowed\n");
		toggle_windowed();
	}
}

void window_while_test()
{
	using namespace dark;

	renderTargetDef = new RenderTarget(width, height, GL_RGB, GL_FLOAT);
	Quadt quad;

	double previousTime = glfwGetTime();
	double lastFrame = 0.0f;
	int frameCount = 0;
	do
	{
		double currentTime = glfwGetTime();
		delta = currentTime - lastFrame;
		lastFrame = currentTime;

		frameCount++;
		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			// Display the frame count here any way you want.
			fps = frameCount;

			frameCount = 0;
			previousTime = currentTime;
		}

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (useFbo)
			renderTargetDef->Bind();
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Group::drawCalls = 0;

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		handle_my_keys();

		glfwSetInputMode(window, GLFW_CURSOR, cursorShowing ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		collision::simulate(delta);

		if (player1)
			player1->Step();
			
		cameraCur->Update(delta);

		if (someDraugr)
			someDraugr->Step();

		if (meanSkelly)
			meanSkelly->Step();

		// if (someDraugr)
		//	someDraugr->Step();

		for (auto mist : mists)
			mist->Misty();

		// if (meanSkelly)
		//	meanSkelly->Step();
		// if (someHuman)
		//	someHuman->Step();

		if (ginterior)
			ginterior->update();

		itemfinder::consider();

		sceneDef->DrawItems();

		Material::Unuse(nullptr, nullptr);

		if (useFbo)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			quad.Draw(renderTargetDef);
		}

		imgui_calls();

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

#if 0
void window_while()
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

	sceneDef->bigGroup->Add(personCam->groupDrawer);

#if 0
	pointlight *myself = new pointlight;
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

		capture_keys();

		handle_my_keys();

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
#endif
