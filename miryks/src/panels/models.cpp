#include <miryks/miryks.h>

#include <files.h>

#include <imgui.h>

void overlay_models()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(450 * 3, 0));
	ImGui::Begin("Models", nullptr, flags);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

	for (const auto &[key, value] : miryks::access_nis())
	{
		NIF ni = value;

		//if (!ni->path)
		//continue;

		if (ImGui::TreeNode(ni->path))
		{
			ImGui::Text("key # %i", key);
			ImGui::Separator();
			if (ImGui::TreeNode("Header"))
			{
				char s[500];
				nif_print_hedr(ni, s);
				ImGui::TextWrapped(s);
				if (ImGui::TreeNode("Block Types"))
				{
					const int n = ni->hdr->num_block_types;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%i %s", i, ni->hdr->block_types[i]);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Block Type Index"))
				{
					const int n = ni->hdr->num_blocks;
					for (int i = 0; i < n; i++)
					{
						unsigned short h = ni->hdr->block_type_index[i];
						char *s = ni->hdr->block_types[h];
						ImGui::Text("%s [%hu]", s, h);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Block Sizes"))
				{
					const int n = ni->hdr->num_blocks;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%u", ni->hdr->block_sizes[i]);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Strings"))
				{
					const int n = ni->hdr->num_strings;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%s", ni->hdr->strings[i]);
					}
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Blocks"))
			{
				const int n = ni->hdr->num_blocks;
				for (int i = 0; i < n; i++)
				{
					//ni_block_t *block = &ni->blocks[i];
					const char *block_type = ni->hdr->block_types[ni->hdr->block_type_index[i]];
					const char *fmt = std::to_string(i).c_str();
					char buf[100];
					sprintf(buf, "%i %s", i, block_type);
					if (ImGui::TreeNode(buf))
					{
						char s[1500];
						nif_print_block(ni, i, s);
						if (s[0] != '\0')
							ImGui::TextWrapped(s);
						else
							ImGui::TextDisabled("not printed");
						ImGui::Separator();
						ImGui::TreePop();
					}
				}
				ImGui::Separator();
				ImGui::TreePop();
			}

			if (ImGui::Button("View"))
			{
				miryks::view_in_place(miryks::get_res(ni->path, ""));
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}