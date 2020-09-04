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

char *get_skyrim_grup(Grup *grup)
{
	if (esp->header->head->flags & 0x00000001)
	{
		if (grup->id == 30)
		{
			return "(STATS)";
		}
	}
	return "";
}

void im_grup(Grup *);
void im_record(Record *);
void im_subrecord(Subrecord *);

void im_grup(Grup *grup)
{
	char t[100];
	snprintf(t, 100, "GRUP %i %s", grup->id, get_skyrim_grup(grup));
	if (ImGui::TreeNode(t))
	{
		char s[100];
		esp_print_grup(esp, s, grup);
		ImGui::Text(s);
		for (int i = 0; i < grup->mixed.used; i++)
		{
			void *element = grup->mixed.pointers[i];
			if (*(enum espnum *)element == GRUP)
				im_grup((Grup *)element);
			if (*(enum espnum *)element == RECORD)
				im_record((Record *)element);
		}
		if (grup->mixed.used)
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
		for (int i = 0; i < record->fields.used; i++)
		{
			im_subrecord((Subrecord *)record->fields.pointers[i]);
		}
		ImGui::TreePop();
	}
}

void im_subrecord(Subrecord *subrecord)
{
	char t[100];
	snprintf(t, 100, "%.4s##Sub %i", (char *)&subrecord->head->type, subrecord->id);
	if (ImGui::TreeNode(t))
	{
		char s[400];
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
		if (ImGui::BeginTabItem("plain"))
		{
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			//if (ImGui::TreeNode("Header"))
			//{
			im_record(esp->header);
			//ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Grups"))
			//{
			for (int i = 0; i < esp->grups.used; i++)
				im_grup((Grup *)esp->grups.pointers[i]);
			//ImGui::TreePop();
			//}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("filter"))
		{
			static esp_array *filtered = NULL;
			static char filter[5] = "NPC_";
			static char buf[5] = {'\0'};
			ImGui::InputText("##espfilter", filter, 5);
			if (strcmp(filter, buf))
			{
				printf("esp try filter %.4s\n", filter);
				memcpy(buf, filter, 4);
				if (filtered)
					esp_free_array(&filtered);
				if (strlen(filter) == 4)
				{
					filtered = esp_filter_records(esp, filter);
				}
			}
			if (filtered)
			{
				ImGui::Text("Found %i records", filtered->used);
				//ImGui::Text("%i", esp->statics.used);
				const ImGuiWindowFlags child_flags = 0;
				const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
				const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
				for (int i = 0; i < filtered->used; i++)
					im_record((Record *)filtered->pointers[i]);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		/*if (ImGui::BeginTabItem("statics"))
		{
			//ImGui::Text("%i", esp->statics.used);
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			for (int i = 0; i < esp->statics.used; i++)
				im_record((Record *)esp->statics.pointers[i]);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}*/
		if (ImGui::BeginTabItem("formIds"))
		{
#if 0
			//ImGui::Text("%i", esp->formIds.used);
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			for (int i = 0; i < esp->formIds.used; i++)
			{
				struct form_id *formId = &esp->formIds.formIds[i];
				if (ImGui::TreeNode(formId->hex))
				{

					ImGui::TreePop();
				}
			}
			ImGui::EndChild();
#endif
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}