#include <libs>
#include <Gloom/Dark2.h>
#include <Gloom/Files.h>

using namespace gloom;

#include <imgui.h>

static char hedrstr[600] = { "not loaded" };

Archive *bsa = NULL;

void bsa_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin("Bsa", nullptr, flags);

#define MAX 230
	static char buf[MAX] = "Skyrim - Meshes.bsa";
	static char buf2[MAX] = {'\0'};
	ImGui::InputText("##archive", buf, IM_ARRAYSIZE(buf));

	if (strcmp(buf, buf2))
	{
		printf("bsa-gui manual loading new archive\n");
		memcpy(buf2, buf, MAX);
		std::string oldrim = pathToOldrim + "Data/" + buf;
		std::string bin = buf;
		std::string *path = nullptr;
		if (exists_test3(oldrim))
			path = &oldrim;
		else if (exists_test3(bin))
			path = &bin;
		if (path)
		{
			// if we had one loaded via gui, see if its part of game
			if (bsa && !bsa->loadlisted)
			{
				printf("unloading non load ordered %s\n", bsa->path);
				bsa_free(&bsa);
				printf("bsa after bsa_free: %i\n", (int) bsa);
			}
			bsa = bsa_get(path->c_str());
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
	if (ImGui::BeginTabBar("BSATabs", tabBarFlags))
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

			static Rc *rc = nullptr;

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

			static const char *items[BSA_MAX_SEARCHES];
			static int num = 0;

			static Rc *rc = nullptr;
			static Rc *rcs[BSA_MAX_SEARCHES];

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

			char found[100];
			snprintf(found, 100, "search returned %i results out of configured max %i", num, BSA_MAX_SEARCHES);
			ImGui::Text(found);

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
				if (ImGui::Button(READ_BSA_RESOURCE))
				{
					bsa_read(rc);
				}
				if (rc->size > -1)
				{
					ImGui::SameLine();
					if (ImGui::Button(VIEW_NIF))
					{
						View(rc);
					}
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("list"))
		{
			//ImGui::Text("Folders %i", bsa->hdr.folders);
			//ImGui::Text("Files %i", bsa->hdr.files);

			const ImGuiWindowFlags child_flags = 0;
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)0);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
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
							Rc *rc = bsa->rc[bsa->r[i] + j];
							bsa_print_rc(bsa, s, rc->r);
							ImGui::Text(s);
							bsa_print_fle_rcd(bsa, s, i, j);
							ImGui::Text(s);

							if (ImGui::Button(READ_BSA_RESOURCE))
							{
								bsa_read(rc);
							}
							if (rc->size > -1)
							{
								ImGui::SameLine();
								if (ImGui::Button(VIEW_NIF))
								{
									View(rc);
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

	ImGui::End();
}