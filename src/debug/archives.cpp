
#include <dark/dark.h>
#include <dark/files.h>

using namespace dark;

#include <imgui.h>

static char hedrstr[600] = {"not loaded"};

Bsa *bsa = NULL;

void bsa_gui()
{
    ImGuiIO& io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(ImVec2(450, 0));
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin("Archive", nullptr, flags);

	static bool reset = false;

#define MAX 230
	static char buf[MAX] = "Skyrim - Meshes0.bsa";
	static char buf2[MAX] = {'\0'};

	ImGui::InputText("##archive", buf, IM_ARRAYSIZE(buf));

	if (strcmp(buf, buf2))
	{
		memcpy(buf2, buf, MAX);
		Bsa *replace = load_archive(buf2);
		if (replace)
		{
			bsa = replace;
			reset = true;
		}
		//if (bsa && bsa->unimportant) // todo also check if its unused
		//{
		//	printf("unloading unimportant %s\n", bsa->path);
		//	bsa_free(&bsa);
		//}
		if (bsa != NULL)
			bsa_print_hedr(bsa, hedrstr);
	}

	ImGui::Separator();

	if (bsa)
	{
		ImGui::Text(bsa->filename);
	}

	if (!bsa)
	{
		ImGui::End();
		return;
	}

	auto func = [&](Rsc *rsc) {
		if (!rsc)
			return;
		ImGui::Separator();
		char s[200];

		bsa_print_rc(bsa, s, rsc->r);
		ImGui::Text(s);

		if (ImGui::TreeNode("more data"))
		{
			bsa_print_fle_rcd(bsa, s, rsc->i, rsc->j);
			ImGui::Text(s);

			bsa_print_fld_rcd(bsa, s, rsc->i);
			ImGui::Text(s);

			ImGui::TreePop();
		}

		if (ImGui::Button("Read"))
		{
			bsa_read(rsc);
		}
		if (rsc->size > 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("View"))
			{
				Nif *model = load_model(rsc, true);
				simple_viewer(model);
			}
		}

		//ImGui::Text("file-folder data:");
		//ImGui::Separator();
	};

	//ImGui::Text(bsa->path);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("BSATabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("header"))
		{
			ImGui::Text(hedrstr);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("find"))
		{
			static char str[MAX] = "meshes\\clutter\\bucket02a.nif";
			static char str2[MAX] = {'\0'};

			static Rsc *rsc = nullptr;

			if (reset)
			{
				str2[0] = '\0';
				rsc = nullptr;
			}

			ImGui::InputText("##Find", str, MAX);

			bool rcb = !!rsc;

			ImGui::Text(rsc ? "found!" : "not found!");
			ImGui::SameLine();
			ImGui::Checkbox("##", &rcb);

			if (strcmp(str, str2))
			{
				rsc = bsa_find(bsa, str);
				memcpy(str2, str, MAX);
			}

			if (rsc)
			{
				func(rsc);
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("search"))
		{
			static char str[MAX] = "bucket";
			static char str2[MAX] = { '\0' };

			if (reset)
				str2[0] = '\0';

			ImGui::InputText("##Search", str, MAX);

			static const char *items[BSA_MAX_SEARCHES];
			static int num = 0;

			static Rsc *rsc = nullptr;
			static Rsc *rcs[BSA_MAX_SEARCHES];

			if (reset)
			{
				num = 0;
				rsc = nullptr;
			}

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
			if (reset)
				item_current = 0;
			ImGui::ListBox("##Results", &item_current, items, num, 10);

			char found[100];
			snprintf(found, 100, "search returned %i results out of configured max %i", num, BSA_MAX_SEARCHES);
			ImGui::Text(found);

			if (!(item_current > num))
			{
				rsc = rcs[item_current];

				func(rsc);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("list"))
		{
			//ImGui::Text("Folders %i", bsa->hdr.folders);
			//ImGui::Text("Files %i", bsa->hdr.files);

			auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);

			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, vec, true, child_flags);
			//ImGui::BeginChildFrame(1, ImVec2(0, 800));
			for (unsigned int i = 0; i < bsa->hdr.folders; i++)
			{
				if (ImGui::TreeNode(bsa->ca[i]))
				{
					char s[200];
					bsa_print_fld_rcd(bsa, s, i);
					ImGui::Text(s);
					ImGui::Separator();
					ImGui::Text("Files:");
					int r = bsa->r[i];
					for (unsigned int j = 0; j < bsa->fld[i].num; j++)
					{
						if (ImGui::TreeNode(bsa->cb[r]))
						{
							ImGui::Separator();

							char s[200];
							Rsc *rsc = bsa->rsc[bsa->r[i] + j];
							bsa_print_rc(bsa, s, rsc->r);
							ImGui::Text(s);
							bsa_print_fle_rcd(bsa, s, i, j);
							ImGui::Text(s);

							if (ImGui::Button("Read"))
							{
								bsa_read(rsc);
							}
							if (rsc->size > -1)
							{
								ImGui::SameLine();
								if (ImGui::Button("View"))
								{
									Nif *model = load_model(rsc, true);
									simple_viewer(model);
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
			//ImGui::EndChildFrame();
			ImGui::EndTabItem();
			ImGui::EndChild();
		}

		ImGui::EndTabBar();
	}

	reset = false;

	ImGui::End();
}