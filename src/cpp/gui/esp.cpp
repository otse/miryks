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

static Esp *esp = NULL;

void esp_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowPos(ImVec2(450, 0));

	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin(ESP_GUI, nullptr, flags);

#define MAX 230
	static char buf[MAX] = "Skyrim.esm";
	static char buf2[MAX] = {'\0'};
	ImGui::InputText("##archive", buf, IM_ARRAYSIZE(buf));

	if (strcmp(buf, buf2))
	{
		printf("esp-gui loading new plugin\n");
		memcpy(buf2, buf, MAX);
		std::string oldrim = OLDRIM + "Data/" + buf;
		std::string bin = buf;
		std::string *path = nullptr;
		if (exists_test3(oldrim))
			path = &oldrim;
		else if (exists_test3(bin))
			path = &bin;
		if (path)
		{
			if (esp != NULL)
			{
				printf("todo unload non load ordered esp here\n");
				esp_free(&esp);
			}
			if (esp == NULL)
				esp = esp_load(path->c_str());
			//if (bsa != NULL)
			//esp_print_hedr(bsa, hedrstr);
		}
	}

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

	auto im_record = [&](Record *record) {
		char s[200];
		esp_print_record(esp, s, record);
		ImGui::Text(s);
		if (ImGui::TreeNode("Subrecords"))
		{
		/*for (int i = 0; i < record->count; i++)
		{
			char s[100];
			snprintf(s, 100, "Subrecord %i", i);
			if (ImGui::TreeNode(s))
			{
				char s[400];
				esp_print_subrecord(esp, s, record->subrecords[i]);
				ImGui::Text(s);
				ImGui::Separator();
				ImGui::TreePop();
			}
		}*/
		ImGui::TreePop();
		}
	};

	if (ImGui::BeginTabBar("EspTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("records"))
		{
			if (ImGui::TreeNode("00"))
			{
				//im_record(esp->header);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}