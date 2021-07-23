#include <lib.h>
#include <dark/dark.h>
#include <core/files.h>

#include "gooey.h"

using namespace dark;

#include <sstream>
#include <imgui.h>

void hdr_gui(bool *open)
{
	if (!*open)
		return;

    ImGuiIO& io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always, ImVec2(1, 1));
	ImGui::SetNextWindowSize(ImVec2(600, 600));

	ImGui::Begin("Hdr", open, flags);

	ImGui::Text("(F10)");

	ImGui::End();

	ImGui::PopStyleColor(4);
}