#include <gloom/dark2.h>
#include <gloom/files.h>
#include <libs>

using namespace gloom;

#include <sstream>
#include <imgui.h>

void cell_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(1350, 0));

	ImGui::Begin("Cell", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellTabs", tabBarFlags))
	{
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