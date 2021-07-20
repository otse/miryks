#include <libs>

#include <Gloom/Gloom.h>
#include <Gloom/Files.h>

#include <Gloom/Image.h>

#include <gooey/gooey.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>

using namespace gloom;

static bool loading = true;

static bool checked = false;

#include <windows.h>

void simple_loader()
{
	static Image *yagrum = new Image();
	static bool first = true;
	if (first)
	{
		yagrum->from_resourcefile();
		first = false;
	}

	static const char *plugins[] = {PLUGIN_ONE, PLUGIN_NAMESAKE};
	static const char *archives[] = {ARCHIVE_ONE, ARCHIVE_TWO, ARCHIVE_THREE, ARCHIVE_FOUR, ARCHIVE_FIVE, ARCHIVE_SIX};

	const int PLUGINS = sizeof(plugins) / sizeof(plugins[0]);
	const int ARCHIVES = sizeof(archives) / sizeof(archives[0]);

	if (!loading)
		return;

	ImGuiIO &io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(800, 0));
	ImGui::SetNextWindowPos(ImVec2(300, 300));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Simple Loader", nullptr, flags);

	ImGui::PushFont(font2);
	// ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	// ImGui::TextWrapped("Gloom ");
	// ImGui::PopStyleColor(1);
	//ImGui::TextWrapped("\n(bucket02a.nif should become visible.)\n\n");
	ImGui::TextWrapped("Let's init.");

	ImGui::SameLine();

	ImRotateStart();
	ImGui::Image((void *)(intptr_t)yagrum->texture, ImVec2(50, 50));
	ImRotateEnd(0.0005f*::GetTickCount(), ImRotationCenter());

	ImGui::NewLine();

	static int plugin = -1;
	static int archive = -1;

	static double passed = glfwGetTime();
	double now = glfwGetTime();
	double elapsed = now - passed;
	static double delay = 1.0 / 1;

	if (elapsed > delay)
	{
		if (plugin == -1)
		{
			plugin = 0;
			delay = 0;
		}
		else if (plugin < PLUGINS)
		{
			delay = 1.0 / 2;
			get_plugins()[plugin] = load_plugin(plugins[plugin]);
			printf("loaded %s!\n", plugins[plugin]);
			plugin++;
		}
		else if (archive == -1)
		{
			archive = 0;
			delay = 1.0 / 4;
		}
		else if (archive < ARCHIVES)
		{
			get_archives()[archive] = load_archive(archives[archive]);
			archive++;
		}
		passed = glfwGetTime();
	}

	static bool bucketed = false;
	if (plugin >= PLUGINS && archive >= ARCHIVES && !bucketed)
	{
		load_bucket();
		load_gloomgen();
		bucketed = true;
	}

	//ImGui::TextWrapped("Plugins");
	//ImGui::Separator();

	for (int i = 0; i < PLUGINS; i++)
	{
		if (i >= plugin)
			ImGui::TextDisabled("%s", plugins[i]);
		else
			ImGui::Text("%s", plugins[i]);
	}

	ImGui::NewLine();
	//ImGui::TextWrapped("Archives");
	//ImGui::Separator();
	//ImGui::NewLine();

	for (int i = 0; i < ARCHIVES; i++)
	{
		if (i >= archive)
			ImGui::TextDisabled("%s", archives[i]);
		else
			ImGui::Text("%s", archives[i]);
	}

	ImGui::PopFont();

	ImGui::PopStyleColor(4);

	ImGui::End();
}
