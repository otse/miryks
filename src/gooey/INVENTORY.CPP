#include <lib.h>

#include <dark/dark.h>
#include <dark/itemrenderer.h>

#include <core/files.h>

#include "../gooey/gooey.h"

using namespace dark;

#include <sstream>
#include <imgui.h>


void inventory_menu()
{
	return;
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::SetNextWindowPos(ImVec2(300, 350));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Inventory Menu", nullptr, flags);

	ImGui::PushFont(font2);
	ImGui::TextWrapped("Welcome to the inventory menu, accessed with <I>.");

    ImGui::BeginGroup();

	//ImVec2 size(40, 40);
	ImGui::Button("ITEM", ImVec2(100, 100));

	ImGui::EndGroup();

	ImGui::PopFont();

	ImGui::PopStyleColor(4);
	
	ImGui::End();
}