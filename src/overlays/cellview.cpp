

#include <dark/dark.h>
#include <dark/files.h>

#include <skyrim/grup.h>
#include <skyrim/cell.h>

using namespace dark;

#include <sstream>
#include <imgui.h>

void overlay_cellview()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(450 * 4, 0));

	ImGui::Begin("Cell View", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellViewTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("interiors"))
		{
			static const char *items[1] = {
				"GloomGen"};

			static int num = 1;
			static int current = 0;

			ImGui::ListBox("##interiors", &current, items, num, 4);

			ImGui::Separator();

			if (!dungeon || dungeon && dungeon->editor_id(items[current]) == false)
			{
				if (ImGui::Button("Load"))
				{
					if (dungeon)
						delete dungeon;
					dungeon = GetInterior(items[current], 5);
					dungeon->Init();
				}
			}
			//if (items[current] == ginterior->loadedCell)
			{
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("world space"))
		{
			if (gworldSpace) {
				ImGui::Text("num total exterior cells: %i", gworldSpace->exteriors.size());
			}
			if (ImGui::Button("Go outdoors?"))
			{
				if (dungeon)
				{
					delete dungeon;
					dungeon = nullptr;
				}
				if (!gworldSpace)
				{
					load_darkworld();
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}