#include "bsa"

#include "dark2.h"

#include "files.hpp"

using namespace dark2;

#include <imgui.h>

#define BSA_GUI "bsa gui"

void bsa_gui()
{

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoTitleBar;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(BSA_GUI, nullptr, flags);

	static bsa_t bsa;
	static bool good = true;
	static stringstream ss;
	static string hedr = "not loaded";

 	static char buf[32] = "Data/Skyrim - Meshes.bsa";
 	static char buf_before[32];
	 
	ImGui::InputText("#archive", buf, IM_ARRAYSIZE(buf));

	if (strcmp(buf, buf_before) && fstat(OLDRIM_PATH + buf)) {
		log_("bsa gui loading archive");
		memcpy(buf_before, buf, 32);
		bsa = bsa_load(OLDRIM_PATH + buf);
		bsa_print_hedr(bsa, ss);
		good = true;
		hedr = ss.str();
		ss.str(string());
		ss.clear();
	}

	if (!good)
		return;

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tabBarFlags)) {
			if (ImGui::BeginTabItem("hedr")) {
				ImGui::Text(hedr.c_str());
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("records")) {
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("rc")) {
				static char rc_str[32] = "meshes\\clutter\\bucket02a.nif";
 				static char rc_str_before[32];
				ImGui::InputText("#rc", rc_str, IM_ARRAYSIZE(rc_str));
				bool find = bsa_find(bsa, rc_str);
				ImGui::Text(find ? "found" : "not found");

				ImGui::EndTabItem();
			}
	}

	//ImGui::EndTabBar();

	ImGui::End();
}