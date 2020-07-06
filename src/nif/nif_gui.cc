#include "dark2.h"
#include "nif"

#include "files"

using namespace dark2;

#include <imgui.h>

#define NIF_GUI "nif"

#define cls           \
	ss.str(string()); \
	ss.clear();

void nif_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(NIF_GUI, nullptr, flags);
	ImGui::End();
}