#include <libs>

#include <Gloom/Gloom.h>
#include <Gloom/Files.h>

#include <Gloom/Image.h>

#include <Gooey/Gooey.h>
#include <Gooey/Yagrum.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>

using namespace gloom;

static bool loading = true;

static bool checked = false;

void simple_loader()
{
	static bool bigger_screen = false;
	static bool loading_dungeon = false;
	static bool successful = false;

	static bool first = true;

	if (first)
	{
		yagrum::queue("Let's have it", 15.0, true);
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

	ImGui::NewLine();

	static int plugin = -1;
	static int archive = -1;

	ImGui::NewLine();

	static double old = glfwGetTime();
	double now = glfwGetTime();
	double elapsed = now - old;
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
		else if (!successful)
		{
			delay = 2.0;
			successful = true;
			yagrum::force_fade();
			yagrum::queue("", 4.0, true);
			yagrum::set_rotate_speed(3.0);
			//yagrum::queue("Going fullscreen windowed...", 5.0f);
		}
		else if (!bigger_screen)
		{
			delay = 1.0;
			//yagrum::force_fade();
			put_it_fullscreen();
			bigger_screen = true;
		}
		else if (!loading_dungeon)
		{
			delay = 0;
			loading_dungeon = true;
			load_gloomgen();
			loading = false;
		}

		old = glfwGetTime();
	}

	if (!successful)
	{
		for (int i = 0; i < PLUGINS; i++)
		{
			if (i >= plugin)
				ImGui::TextDisabled("%s", plugins[i]);
			else
				ImGui::Text("%s", plugins[i]);
		}

		ImGui::NewLine();

		for (int i = 0; i < ARCHIVES; i++)
		{
			if (i >= archive)
				ImGui::TextDisabled("%s", archives[i]);
			else
				ImGui::Text("%s", archives[i]);
		}
	}

	ImGui::PopFont();

	ImGui::PopStyleColor(4);

	ImGui::End();
}
