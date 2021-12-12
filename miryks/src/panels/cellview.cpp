#include <miryks/miryks.hpp>

#include <files.h>
#include <imgui.h>
#include <sstream>

void overlay_cellview()
{
	using namespace miryks;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(450 * 4, 0));

	ImGui::Begin(" View", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellViewTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("interiors"))
		{
			static const char *items[2] = {
				"GloomGen",
				"GloomAfterGen"
			};

			static int num = 2;
			static int current = 0;

			ImGui::ListBox("##interiors", &current, items, num, 4);

			ImGui::Separator();

			if (!ginterior || ginterior && ginterior->editor_id(items[current]) == false)
			{
				if (ImGui::Button("Load"))
				{
					if (ginterior)
						delete ginterior;
					///load_interior(items[current]);
				}
			}
			//if (items[current] == ginterior->loadedCell)
			{
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("world space"))
		{
			if (gworldspace) {
				ImGui::Text("num total exterior cells: %i", gworldspace->exteriors.size());
			}
			if (ImGui::Button("Go outdoors?"))
			{
				if (ginterior)
				{
					delete ginterior;
					ginterior = nullptr;
				}
				if (!gworldspace)
				{
					///load_world_space("DarkWorld");
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}