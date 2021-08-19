
#include <dark/dark.h>
#include <core/files.h>

#include "gooey.h"

using namespace dark;

#include <sstream>
#include <imgui.h>

void render_stats(bool *open)
{
	if (!*open)
		return;

    ImGuiIO& io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings;

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 220, 0, 255));

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 0));

	ImGui::Begin("Stats", open, flags);

	ImGui::Text("(F10)");
	ImGui::Text("window %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
	ImGui::Text("fps %u", fps);
	ImGui::Text("delta %.5f", delta);

	//ImGui::Separator();
	ImGui::End();

	ImGui::PopStyleColor(4);
}