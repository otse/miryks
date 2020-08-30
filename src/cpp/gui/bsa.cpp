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
static char hedrstr[600] = { "not loaded" };
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
				bsa_print_hedr(bsa, hedrstr);
		}
	}

	ImGui::Separator();

	if (!bsa)
	{
		ImGui::End();
		return;
	}

	//ImGui::Text(bsa->path);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("BsaTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("hedr"))
		{
			ImGui::Text(hedrstr);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("find"))
		{
			static char str[MAX] = "meshes\\clutter\\bucket02a.nif";
			static char str2[MAX] = {'\0'};

			static rc_t *rc = nullptr;

			ImGui::InputText("##Find", str, MAX);

			ImGui::Text(rc ? "found!" : "not found!");

			if (strcmp(str, str2))
			{
				rc = bsa_find(bsa, str);
				memcpy(str2, str, MAX);
			}

			if (rc)
			{
				ImGui::Separator();
				char s[200];
				bsa_print_rc(bsa, s, rc->r);
				ImGui::Text(s);
				bsa_print_fle_rcd(bsa, s, rc->i, rc->j);
				ImGui::Text(s);
				bsa_print_fld_rcd(bsa, s, rc->i);
				ImGui::Text(s);
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("search"))
		{
			static char str[MAX] = "staffofmagnus";
			static char str2[MAX] = {'\0'};

			ImGui::InputText("##Search", str, MAX);

			static const char *items[10];
			static int num = 0;

			static rc_t *rc = nullptr;
			static rc_t *rcs[10] = {0};

			if (strcmp(str, str2))
			{
				bsa_search(bsa, rcs, str, &num);
				for (int i = 0; i < num; i++)
				{
					items[i] = rcs[i]->name;
				}
				memcpy(str2, str, MAX);
			}

			static int item_current = 0;
			ImGui::ListBox("##Results", &item_current, items, num, 10);

			rc = rcs[item_current];

			if (!(item_current > num) && rc)
			{
				ImGui::Separator();
				char s[200];
				bsa_print_rc(bsa, s, rc->r);
				ImGui::Text(s);
				bsa_print_fle_rcd(bsa, s, rc->i, rc->j);
				ImGui::Text(s);
				bsa_print_fld_rcd(bsa, s, rc->i);
				ImGui::Text(s);
				if (ImGui::Button("load"))
				{
					bsa_read(rc);
				}
				if (rc->size > -1)
				{
					ImGui::SameLine();
					if (ImGui::Button("view"))
					{
						viewer::spotlight(rc);
					}
				}
				ImGui::Separator();
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
					char s[200];
					bsa_print_fld_rcd(bsa, s, i);
					ImGui::Text(s);
					ImGui::Separator();
					ImGui::Text("Files:");
					int r = bsa->r[i];
					for (uns_t j = 0; j < bsa->fld[i].num; j++)
					{
						if (ImGui::TreeNode(bsa->cb[r]))
						{
							char s[200];
							rc_t *rc = bsa->rc[bsa->r[i] + j];
							bsa_print_rc(bsa, s, rc->r);
							ImGui::Separator();
							ImGui::Text(s);
							bsa_print_fle_rcd(bsa, s, i, j);
							ImGui::Text(s);

							if (ImGui::Button("load"))
							{
								bsa_read(rc);
							}
							if (rc->size > -1)
							{
								ImGui::SameLine();
								if (ImGui::Button("view"))
								{
									viewer::spotlight(rc);
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