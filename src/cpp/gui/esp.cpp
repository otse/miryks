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

void im_grup(Grup *);
void im_record(Record *);
void im_subrecord(Subrecord *);

void im_grup(Grup *grup)
{
	char t[100];
	snprintf(t, 100, "GRUP %i", grup->id);
	if (ImGui::TreeNode(t))
	{
		char s[100];
		esp_print_grup(esp, s, grup);
		ImGui::Text(s);
		for (int i = 0; i < grup->grups.used; i++)
		{
			im_grup((Grup *)grup->grups.array[i]);
		}
		for (int i = 0; i < grup->records.used; i++)
		{
			im_record((Record *)grup->records.array[i]);
		}
		if (grup->grups.used)
			ImGui::Separator();
		ImGui::TreePop();
	}
}

void im_record(Record *record)
{
	char t[100];
	snprintf(t, 100, "%.4s %i", (char *)&record->head->type, record->id);
	if (ImGui::TreeNode(t))
	{
		char s[200];
		esp_print_record(esp, s, record);
		ImGui::Text(s);
		for (int i = 0; i < record->subrecords.used; i++)
		{
			im_subrecord((Subrecord *)record->subrecords.array[i]);
		}
		ImGui::TreePop();
	}
}

void im_subrecord(Subrecord *subrecord)
{
	char t[100];
	snprintf(t, 100, "%.4s %i", (char *)&subrecord->head->type, subrecord->id);
	if (ImGui::TreeNode(t))
	{
		char s[200];
		esp_print_subrecord(esp, s, subrecord);
		ImGui::Text(s);
		ImGui::TreePop();
	}
}

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

	if (ImGui::BeginTabBar("EspTabs", tabBarFlags))
	{
		ImGui::Text("Num group: %u", esp->grups.used);
		if (ImGui::BeginTabItem("records"))
		{
			if (ImGui::TreeNode("Header"))
			{
				im_record(esp->header);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Grups"))
			{
				for (int i = 0; i < esp->grups.used; i++)
					im_grup((Grup *)esp->grups.array[i]);
				ImGui::TreePop();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}