#include "dark2.h"

extern "C"
{
#include "nif.h"
}

#include "files.h"

#include <sstream>
#include <imgui.h>

#define NIF_GUI "nifs"

#define cls           \
	ss.str(string()); \
	ss.clear();

using namespace dark2;

static stringstream ss;

void nif_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::SetNextWindowPos(ImVec2(900, 0));
	ImGui::Begin(NIF_GUI, nullptr, flags);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	
	for (int i = 0; i < nifs; i++)
	{
		Nif *nif = nmap[i].value;

		if (!nif->path)
			continue;

		if (ImGui::TreeNode(nif->path))
		{
			ss << "# " << nif->n << "\nkey " << nmap->key;
			ImGui::Text(ss.str().c_str());
			cls;
			ImGui::Separator();
			if (ImGui::TreeNode("Header"))
			{
				char s[500];
				nif_print_hedr(nif, s);
				ImGui::TextWrapped(s);
				if (ImGui::TreeNode("Block Types"))
				{
					const int n = nif->hdr.num_block_types;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%i %s", i, nif->hdr.block_types[i]);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Block Type Index"))
				{
					const int n = nif->hdr.num_blocks;
					for (int i = 0; i < n; i++)
					{
						unsigned short h = nif->hdr.block_type_index[i];
						char *s = nif->hdr.block_types[h];
						ImGui::Text("%s [%hu]", s, h);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Block Sizes"))
				{
					const int n = nif->hdr.num_blocks;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%u", nif->hdr.block_sizes[i]);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Strings"))
				{
					const int n = nif->hdr.num_strings;
					for (int i = 0; i < n; i++)
					{
						ImGui::Text("%s", nif->hdr.strings[i]);
					}
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Blocks"))
			{
				const int n = nif->hdr.num_blocks;
				for (int i = 0; i < n; i++)
				{
					//ni_block_t *block = &nif->blocks[i];
					const char *block_type = nif->hdr.block_types[nif->hdr.block_type_index[i]];
					const char *fmt = to_string(i).c_str();
					char buf[100];
					sprintf(buf, "%i %s", i, block_type);
					if (ImGui::TreeNode(buf))
					{
						char s[1500];
						nif_print_block(nif, i, s);
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

			if (ImGui::Button(VIEW_NIF))
			{
				viewer::view((struct rc*)nmap[i].key);
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}