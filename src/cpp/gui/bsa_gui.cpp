#include "dark2.h"

extern "C" {
#include "c/c.h"
#include "c/files.h"
#include "c/bsa/bsa.h"
}

using namespace dark2;

#include <sstream>
#include <imgui.h>

#define BSA_GUI "bsa"

#define cls           \
	ss.str(string()); \
	ss.clear();

static bsa_t bsa;
static bool good = true;
static stringstream ss;
static char *hedr = "not loaded";
static char buf[230] = "Data/Skyrim - Meshes.bsa";
static char buf_before[230];

void bsa_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(BSA_GUI, nullptr, flags);
	
	ImGui::InputText("##archive", buf, IM_ARRAYSIZE(buf));
	if (strcmp(buf, buf_before) && fchec((OLDRIM_PATH + buf).c_str()))
	{
		log_("bsa gui loading new archive");
		memcpy(buf_before, buf, 230);
		bsa = bsa_load((OLDRIM_PATH + buf).c_str());
		hedr = bsa_print_hedr(&bsa);
		good = true;
		cls;
	}
	if (!good)
		return;
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
			rc_t *rc = bsa_find(&bsa, rc_str);
			ImGui::Text(rc ? "found!" : "not found!");
			if (rc)
			{
				char *s;
				s = bsa_print_rc(&bsa, rc->r);
				ImGui::Separator();
				ImGui::Text(s);
				free(s);
				s = bsa_print_fle_rcd(&bsa, rc->i, rc->j);
				ImGui::Separator();
				ImGui::Text(s);
				free(s);
				//cls;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("list"))
		{
			ImGui::BeginChildFrame(1, ImVec2(0, 800));
			for (uns_t i = 0; i < bsa.hdr.folders; i++)
			{
				if (ImGui::TreeNode(bsa.ca[i]))
				{
					char *s = bsa_print_fld_rcd(&bsa, i);
					ImGui::Text(s);
					free(s);
					ImGui::Separator();
					//cls;
					ImGui::Text("Files:");
					int r = bsa.r[i];
					for (uns_t j = 0; j < bsa.fld[i].num; j++)
					{
						if (ImGui::TreeNode(bsa.cb[r]))
						{
							char *s = bsa_print_fle_rcd(&bsa, i, j);
							ImGui::Text(s);
							free(s);
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