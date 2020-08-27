#include "dark2.h"
#include "files"

extern "C"
{
#include "c/c.h"
#include "c/files.h"
#include "c/bsa.h"
}

using namespace dark2;

#include <sstream>
#include <imgui.h>

#define BSA_GUI "bsa"

#define cls           \
	ss.str(string()); \
	ss.clear();

static bool good = true;
static stringstream ss;
static char *hedr = "not loaded";
static bsa_t *bsa = NULL;

void bsa_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin(BSA_GUI, nullptr, flags);

#define MAX 230
	static char buf[MAX] = "Data/Skyrim - Meshes.bsa";
	static char buf2[MAX] = {'\0'};
	ImGui::InputText("Manual##archive", buf, IM_ARRAYSIZE(buf));

	if (strcmp(buf, buf2))
	{
		printf("bsa-gui manual loading new archive\n");
		memcpy(buf2, buf, MAX);
		std::string oldrim = OLDRIM + buf;
		std::string bin = buf;
		std::string *path = nullptr;
		if (exists_test3(oldrim))
			path = &oldrim;
		else if (exists_test3(bin))
			path = &bin;
		if (path)
		{
			if (bsa && !bsa->bsas)
			{
				printf("unloading non load ordered %s\n", bsa->path);
				bsa_free(&bsa);
				printf("bsa after bsa_free: %i\n", bsa);
			}
			bsa = bsas_get_by_path(&bsas, path->c_str());
			if (bsa == NULL)
				bsa = bsa_load(path->c_str());
			if (bsa != NULL)
				hedr = bsa_print_hedr(bsa);
		}
	}

	/*const char *items[] = {"Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon"};
	static int item_current = 1;

	ImGui::ListBox("Default", &item_current, items, IM_ARRAYSIZE(items), 4);*/
	ImGui::Separator();

	if (!bsa)
	{
		ImGui::End();
		return;
	}

	//ss << bsa->path;
	ImGui::Text(bsa->path);
	//cls;

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("BsaTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("hedr"))
		{
			ImGui::Text(hedr);
			ImGui::EndTabItem();
		}
		//if (ImGui::BeginTabItem("records"))
		//{
		//	ImGui::EndTabItem();
		//}
		if (ImGui::BeginTabItem("find"))
		{
			static char rc_str[230] = "meshes\\clutter\\bucket02a.nif";
			static char rc_str_before[230];
			ImGui::InputText("##rc", rc_str, IM_ARRAYSIZE(rc_str));
			rc_t *rc = bsa_find(bsa, rc_str);
			ImGui::Text(rc ? "found!" : "not found!");
			if (rc)
			{
				char *s;
				s = bsa_print_rc(bsa, rc->r);
				//ImGui::Separator();
				//ImGui::Text("Resource:");
				ImGui::Text(s);
				free(s);
				s = bsa_print_fle_rcd(bsa, rc->i, rc->j);
				//ImGui::Separator();
				ImGui::Text(s);
				free(s);
				//if (!rc->buf)
				//bsa_read(&bsa, rc);
				if (rc->buf)
				{
					//ImGui::Separator();
					ImGui::Text("Contents:");
					ImGui::Text((char *)rc->buf);
				}
				//cls;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("list"))
		{
			ImGui::BeginChildFrame(1, ImVec2(0, 800));
			for (uns_t i = 0; i < bsa->hdr.folders; i++)
			{
				if (ImGui::TreeNode(bsa->ca[i]))
				{
					char *s = bsa_print_fld_rcd(bsa, i);
					ImGui::Text(s);
					free(s);
					ImGui::Separator();
					//cls;
					ImGui::Text("Files:");
					int r = bsa->r[i];
					for (uns_t j = 0; j < bsa->fld[i].num; j++)
					{
						if (ImGui::TreeNode(bsa->cb[r]))
						{
							char *s;
							rc_t *rc = bsa->rc[bsa->r[i] + j];
							s = bsa_print_rc(bsa, rc->r);
							ImGui::Separator();
							ImGui::Text(s);
							free(s);
							s = bsa_print_fle_rcd(bsa, i, j);
							ImGui::Text(s);
							free(s);

							if (ImGui::Button("load"))
							{
								bsa_read(rc);
							}
							if (rc->size > -1)
							{
								ImGui::SameLine();
								if (ImGui::Button("view"))
								{
									nif_viewer(nif_from_rc(rc));
								}
							}
							ImGui::Separator();
							ImGui::TreePop();
							//cls;
						}
						r++;
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndChildFrame();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}