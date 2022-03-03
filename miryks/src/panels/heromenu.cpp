#include <files.h>

#include <panels.h>

#include <sstream>
#include <imgui.h>

void hero_menu()
{
    ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::SetNextWindowPos(ImVec2(300, 300));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Hero Menu", nullptr, flags);

	ImGui::PushFont(font2);
	ImGui::TextWrapped("Welcome to the hero menu, accessed with <H>.");
	ImGui::Separator();
	ImGui::TextWrapped("\nHealth: 100");
	ImGui::TextWrapped("\nMana: 100");
	ImGui::TextWrapped("\nStamina: 100");

	ImGui::PopFont();

	ImGui::PopStyleColor(4);
	
	ImGui::End();
}