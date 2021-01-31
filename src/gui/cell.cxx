#include <gloom/dark2.h>
#include <gloom/files.h>
#include <libs>

using namespace gloom;

#include <sstream>
#include <imgui.h>

void cell_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(1350, 0));

	ImGui::Begin("cell gui", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("BSATabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("hedr"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("find"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("search"))
		{
			ImGui::EndTabItem();
        }
		if (ImGui::BeginTabItem("list"))
		{
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}