#include <miryks/list>
#include <miryks/libs.h>
#include <files.h>

#include <imgui.h>

BSA archive = NULL;

void overlay_archives()
{
    ImGuiIO& io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(ImVec2(450, 0));
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin("Archive", nullptr, flags);

	static bool reset = false;

	static const char *items[] = {
		ARCHIVE_0,
		ARCHIVE_1,
		ARCHIVE_2,
		ARCHIVE_3,
		ARCHIVE_4,
		ARCHIVE_5,
		ARCHIVE_6,
		ARCHIVE_7,
		ARCHIVE_8,
		ARCHIVE_9,
		ARCHIVE_10,
		ARCHIVE_11,
		ARCHIVE_12,
		ARCHIVE_13,
		ARCHIVE_14,
		ARCHIVE_15,
		ARCHIVE_16,
		ARCHIVE_17
	};

	static int current = 0;
	static int old = -1;

	ImGui::Combo("archives", &current, items, IM_ARRAYSIZE(items));

	if (old != current)
	{
		archive = get_archives()[current];
		if (archive) {
		}
		reset = true;
		old = current;
	}

	ImGui::Separator();

	if (archive)
	{
		ImGui::Text(archive->filename);
	}

	if (!archive)
	{
		ImGui::End();
		return;
	}

	auto func = [&](RES res) {
		if (!res)
			return;
		ImGui::Separator();
		char s[200];

		bsa_print_rc(archive, s, res->r);
		ImGui::Text(s);

		if (ImGui::TreeNode("more data"))
		{
			bsa_print_fle_rcd(archive, s, res->i, res->j);
			ImGui::Text(s);

			bsa_print_fld_rcd(archive, s, res->i);
			ImGui::Text(s);

			ImGui::TreePop();
		}

		if (ImGui::Button("Read"))
		{
			bsa_read(res);
		}
		if (res->size > 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("View"))
			{
				// todo move view in place
				//view_in_place(res);
			}
		}

		//ImGui::Text("file-folder data:");
		//ImGui::Separator();
	};

	//ImGui::Text(archive->path);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("BSATabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("header"))
		{
			if (archive) {
				char hedr[600];
				bsa_print_hedr(archive, hedr);
				ImGui::Text(hedr);
			}
			else
				ImGui::Text("not loaded by dark");
			ImGui::EndTabItem();
		}
		
		if (!archive)
		{
			ImGui::EndTabBar();
			ImGui::End();
			return;
		}

		if (ImGui::BeginTabItem("find"))
		{
			#define MAX 230

			static char str[MAX] = "meshes\\clutter\\bucket02a.nif";
			static char str2[MAX] = {'\0'};

			static RES res = nullptr;

			if (reset)
			{
				str2[0] = '\0';
				res = nullptr;
			}

			ImGui::InputText("##Find", str, MAX);

			bool rcb = !!res;

			ImGui::Text(res ? "found!" : "not found");
			ImGui::SameLine();
			ImGui::Checkbox("##", &rcb);

			if (strcmp(str, str2))
			{
				res = bsa_find(archive, str);
				memcpy(str2, str, MAX);
			}

			if (res)
			{
				func(res);
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

			static RES res = nullptr;
			static RES rcs[BSA_MAX_SEARCHES];

			if (reset)
			{
				num = 0;
				res = nullptr;
			}

			if (strcmp(str, str2))
			{
				bsa_search(archive, rcs, str, &num);
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
				res = rcs[item_current];

				func(res);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("list"))
		{
			//ImGui::Text("Folders %i", archive->hdr.folders);
			//ImGui::Text("Files %i", archive->hdr.files);

			auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);

			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, vec, true, child_flags);
			//ImGui::BeginChildFrame(1, ImVec2(0, 800));
			for (unsigned int i = 0; i < archive->hdr.folders; i++)
			{
				if (ImGui::TreeNode(archive->ca[i]))
				{
					char s[200];
					bsa_print_fld_rcd(archive, s, i);
					ImGui::Text(s);
					ImGui::Separator();
					ImGui::Text("Files:");
					int r = archive->r[i];
					for (unsigned int j = 0; j < archive->fld[i].num; j++)
					{
						if (ImGui::TreeNode(archive->cb[r]))
						{
							ImGui::Separator();

							char s[200];
							RES res = archive->res[archive->r[i] + j];
							bsa_print_rc(archive, s, res->r);
							ImGui::Text(s);
							bsa_print_fle_rcd(archive, s, i, j);
							ImGui::Text(s);

							if (ImGui::Button("Read"))
							{
								bsa_read(res);
							}
							if (res->size > -1)
							{
								ImGui::SameLine();
								if (ImGui::Button("View"))
								{
									// move view in palce
									//view_in_place(res);
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