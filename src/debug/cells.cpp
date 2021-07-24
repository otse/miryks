#include <lib.h>

#include <dark/dark.h>
#include <core/files.h>

#include <skyrim/wrappers.h>
#include <skyrim/interior.h>

using namespace dark;

#include <sstream>
#include <imgui.h>

void cell_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(1350, 0));

	ImGui::Begin("cell", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("cell view"))
		{
			static const char *items[2] = {
				"GloomGen",
				"GloomAfterGen"};

			static int num = 2;
			static int current = 0;

			ImGui::ListBox("##CellView", &current, items, num, 4);

			ImGui::Separator();

			RecordWrapper object = RecordWrapper(dungeon->loaded_cell.record);

			const char *editorId = getEditorId(object);

			if (0 != strcmp(items[current], editorId))
			{
				if (ImGui::Button("Load"))
				{
					delete dungeon;
					dungeon = new Interior(items[current]);
					dungeon->loadcell();
				}
			}
			//if (items[current] == dungeon->loadedCell)
			{
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("refrs"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("by edid?"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("bluh"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("another tab"))
		{
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}