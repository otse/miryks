#include <libs>
#include <list.hpp>

#include <Gloom/Gloom.h>
#include <Gloom/Files.h>

#include <gooey/gooey.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>

using namespace gloom;

static bool loading = true;

static bool checked = false;

void simple_loader()
{
	static const char *plugins[] = {PLUGIN_ONE, PLUGIN_NAMESAKE};
	static const char *archives[] = {ARCHIVE_ONE, ARCHIVE_TWO, ARCHIVE_THREE, ARCHIVE_FOUR, ARCHIVE_FIVE, ARCHIVE_SIX};

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
	ImGui::TextWrapped("\n(bucket02a.nif should become visible.)\n\n");

	ImGui::NewLine();

	static int plugin = -1;
	static int archive = -1;

#define X ""
#define V ""

	static double passed = glfwGetTime();
	double now = glfwGetTime();
	double elapsed = now - passed;
	static double delay = 1.0 / 1;

	if (elapsed > delay)
	{
		if (plugin == -1)
		{
			plugin = 0;
		}
		else if (plugin < 2)
		{
			get_plugins()[plugin] = load_plugin(plugins[plugin]);
			printf("loaded %s!\n", plugins[plugin]);
			plugin++;
		}
		else if (archive == -1)
		{
			archive = 0;
			delay = 1.0 / 2;
		}
		else if (archive < 6)
		{
			get_archives()[archive] = load_archive(archives[archive]);
			archive++;
		}
		passed = glfwGetTime();
	}

	static bool bucketed = false;
	if (plugin >= 2 && archive >= 6 && !bucketed)
	{
		load_bucket();
		bucketed = true;
	}

	//ImGui::TextWrapped("Plugins");
	//ImGui::Separator();

	for (int i = 0; i < 2; i++)
	{
		const char *here = plugin == i ? " <" : "";
		if (i >= plugin)
			ImGui::TextDisabled(X " %s %s", plugins[i], here);
		else
			ImGui::Text(V " %s", plugins[i]);
	}

	ImGui::NewLine();
	//ImGui::TextWrapped("Archives");
	//ImGui::Separator();
	//ImGui::NewLine();

	for (int i = 0; i < 6; i++)
	{
		const char *here = archive == i ? "<" : "";
		if (i >= archive)
			ImGui::TextDisabled(X " %s %s", archives[i], here);
		else
			ImGui::Text(V " %s", archives[i]);
	}

	ImGui::PopFont();

	ImGui::PopStyleColor(4);

	ImGui::End();
}
