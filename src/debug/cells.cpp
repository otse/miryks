

#include <dark/dark.h>
#include <dark/files.h>

#include <skyrim/grup.h>
#include <skyrim/interior.h>

using namespace dark;

#include <sstream>
#include <imgui.h>

void cell_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(450 * 4, 0));

	ImGui::Begin("Interiors", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("cell view"))
		{
			static const char *items[3] = {
				"GloomGen",
				"GloomAfterGen",
				"DarkTelvanni"
			};

			static int num = 3;
			static int current = 0;

			ImGui::ListBox("##CellView", &current, items, num, 4);

			ImGui::Separator();

			const char *edId = "";

			if (dungeon)
				edId = dungeon->cell.wrcd.editorId();

			if (0 != strcmp(items[current], edId))
			{
				if (ImGui::Button("Load"))
				{
					delete dungeon;
					dungeon = new Interior(items[current]);
					dungeon->Load();
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