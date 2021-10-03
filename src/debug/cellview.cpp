

#include <dark/dark.h>
#include <dark/files.h>

#include <skyrim/grup.h>
#include <skyrim/cell.h>

using namespace dark;

#include <sstream>
#include <imgui.h>

void cell_gui()
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
				"GloomGen"
			};

			static int num = 1;
			static int current = 0;

			ImGui::ListBox("##interiors", &current, items, num, 4);

			ImGui::Separator();

			if (ginterior && ginterior->cell.wrcd.editor_id(items[current]) == false)
			{
				if (ImGui::Button("Load"))
				{
					delete ginterior;
					ginterior = new Interior(items[current]);
					ginterior->Load();
				}
			}
			//if (items[current] == ginterior->loadedCell)
			{
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("world space"))
		{
			ImGui::TextDisabled("( DarkWorld World-Space )");
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}