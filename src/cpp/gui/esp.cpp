#include "dark2.h"
#include "files"

extern "C"
{
#include "c/files.h"
#include "c/c.h"
#include "c/esp.h"
}

using namespace dark2;

#include <sstream>
#include <imgui.h>

#define ESP_GUI "esp"

static stringstream ss;

void esp_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(-450, 0));
	ImGui::Begin(ESP_GUI, nullptr, flags);

	ImGui::Text("Bep");


	ImGui::End();
}