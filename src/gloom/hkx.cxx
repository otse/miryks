#include <libs>

#include <gloom/files.h>
#include <gloom/actor.h>
#include <gloom/skeleton.h>
#include <gloom/level.h>
#include <gloom/mesh.h>

#include <opengl/group.h>
#include <opengl/scene.h>
#include <opengl/camera.h>

namespace gloom
{
	KeyFrames *draugrAttack = nullptr;
	KeyFrames *humanIdle = nullptr;

	void ExportRaceHkxToKf(const char *raceId)
	{
		if (strcmp(raceId, "DraugrRace") == 0)
		{
			fmkdir("temp/");
			fmkdir("temp/draugr");
			fmkdir("temp/draugr/hkx");
			fmkdir("temp/draugr/kf");

			if (!exists("temp/draugr/hkx/skeleton.hkx"))
			{
				Bsa *animations = get_archives()[2];
				Rc *skeleton = bsa_find(animations, "meshes\\actors\\draugr\\character assets\\skeleton.hkx");

				if (skeleton)
					printf("hkxcmd for draugr skeleton.hkx!\n");

				const char *fld = "meshes\\actors\\draugr\\animations";
				unsigned int i = 0;
				int good = 0;
				for (; i < animations->hdr.folders; i++)
				{
					int cmp = strcmp(fld, animations->ca[i]);
					if (cmp == 0)
					{
						good = 1;
						break;
					}
				}

				int r = animations->r[i];
				for (unsigned int j = 0; j < animations->fld[i].num; j++)
				{
					Rc *rc = animations->rc[r];
					bsa_read(rc);
					char path[255];
					strcpy(path, "temp/draugr/hkx/");
					strcat(path, "/");
					strcat(path, animations->cb[r]);
					cfout2(path, rc->buf, rc->size);
					//printf("ok %s\n", animations->cb[r]);
					r++;
				}
				cassert(good, "cant find bsa fld for hkxtokf race anims");

				bsa_read(skeleton);

				cfout2("temp/draugr/hkx/skeleton.hkx", skeleton->buf, skeleton->size);

				printf("Sec, exporting hkx to kf");
				system("hkxcmd.exe exportkf \"temp/draugr/hkx/skeleton.hkx\" \"temp/draugr/hkx/\" \"temp/draugr/kf\"");

				if (skeleton)
					printf("draugr hkx skeleton found %i\n", (int) skeleton);
			}
			else
			{
				const char *path = "temp/draugr/kf/1hmattackf.kf";
				printf("fetching random draugr kf\n");
				Nifp *nif = malloc_nifp();
				nif->path = path;
				fbuf(path, &(char *)nif->buf);
				nifp_read(nif);
				nifp_save(nif, nif);
				draugrAttack = new KeyFrames(nif);
			}
		}

		if (strcmp(raceId, "ImperialRace") == 0)
		{
			fmkdir("temp/");
			fmkdir("temp/character");
			fmkdir("temp/character/hkx");
			fmkdir("temp/character/kf");

			if (!exists("temp/character/hkx/skeleton.hkx"))
			{
				Bsa *animations = get_archives()[2];
				Rc *skeleton = bsa_find(animations, "meshes\\actors\\character\\character assets\\skeleton.hkx");

				if (skeleton)
					printf("found character.hkx for character / imperial race\n");

				const char *fld = "meshes\\actors\\character\\animations";
				unsigned int i = 0;
				int good = 0;
				for (; i < animations->hdr.folders; i++)
				{
					int cmp = strcmp(fld, animations->ca[i]);
					if (cmp == 0)
					{
						good = 1;
						break;
					}
				}

				int r = animations->r[i];
				for (unsigned int j = 0; j < animations->fld[i].num; j++)
				{
					Rc *rc = animations->rc[r];
					bsa_read(rc);
					char path[255];
					strcpy(path, "temp/character/hkx/");
					strcat(path, "/");
					strcat(path, animations->cb[r]);
					cfout2(path, rc->buf, rc->size);
					//printf("ok %s\n", animations->cb[r]);
					r++;
				}
				cassert(good, "cant find bsa fld for hkxtokf race anims");

				bsa_read(skeleton);

				cfout2("temp/character/hkx/skeleton.hkx", skeleton->buf, skeleton->size);

				printf("Sec, exporting hkx to kf");
				system("hkxcmd.exe exportkf \"temp/character/hkx/skeleton.hkx\" \"temp/character/hkx/\" \"temp/character/kf\"");

				if (skeleton)
					printf("skeleton hkx skeleton found %i\n", (int) skeleton);
			}
			else
			{
				const char *path = "temp/character/kf/1hm_idle.kf";
				printf("fetching human idle kf\n");
				Nifp *nif = malloc_nifp();
				nif->path = path;
				fbuf(path, &(char *)nif->buf);
				nifp_read(nif);
				nifp_save(nif, nif);
				humanIdle = new KeyFrames(nif);
			}
		}
	}

} // namespace gloom