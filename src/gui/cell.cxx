#include <Gloom/Dark2.h>
#include <Gloom/Files.h>
#include <Gloom/Object.h>
#include <Gloom/Interior.h>
#include <libs>

using namespace gloom;

#include <sstream>
#include <imgui.h>

void cell_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(1350, 0));

	ImGui::Begin("Cell", nullptr, flags);

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

			Object object(dungeon->loadedCell.record);

			const char *editorId = GetEditorId(object);

			if (0 != strcmp(items[current], editorId))
			{
				if (ImGui::Button("Load"))
				{
					delete dungeon;
					dungeon = new Interior(items[current]);
					dungeon->LoadCell();
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