#include <gloom/libs>

#include <gloom/files.h>
#include <gloom/actor.h>
#include <gloom/skeleton.h>
#include <gloom/level.h>
#include <gloom/mesh.h>

#include <opengl/group.h>
#include <opengl/scene.h>

namespace gloom
{
	Object GetRace(const char *raceId)
	{
		grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

		cassert(top, "no race top grup");

		for (size_t i = 0; i < top->mixed.size; i++)
		{
			if (strcmp(Object(top->mixed.records[i], 1).Get<char *>("EDID"), raceId) == 0)
				return Object(top->mixed.records[i], 0);
		}
		cassert(false, "GetRace");
		return Object(nullptr, 0);
	}

	KeyFrames *draugrAttack = nullptr;

	void ExportRaceHkxToKf(const char *raceId)
	{
		if (strcmp(raceId, "DraugrRace") == 0)
		{
			if (!exists("temp/draugr/hkx/skeleton.hkx"))
			{
				bsa *animations = get_archives()[2];
				rc *skeleton = bsa_find(animations, "meshes\\actors\\draugr\\character assets\\skeleton.hkx");

				if (skeleton)
					printf("hkxcmd for draugr skeleton.hkx!\n");

				const char *fld = "meshes\\actors\\draugr\\animations";
				int i = 0;
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
				for (int j = 0; j < animations->fld[i].num; j++)
				{
					rc *rc = animations->rc[r];
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
					printf("draugr hkx skeleton found %i\n", skeleton);
			}
			else
			{
				const char *path = "temp/draugr/kf/1hmattackf.kf";
				printf("fetching random draugr kf\n");
				struct nifp *nif = malloc_nifp();
				nif->path = path;
				fbuf(path, &(char *)nif->buf);
				nifp_read(nif);
				nifp_save(nif, nif);
				draugrAttack = new KeyFrames(nif);
			}
		}

		// we exported the hkx to kf


		/*
		pseudo section!

		*/
	}

	Actor::Actor(const char *raceId, const char *model)
	{
		printf("actor of race %s\n", raceId);

		Object race = GetRace(raceId);

		ExportRaceHkxToKf(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.Get<char *>("ANAM", 0);

		auto MODL = race.Get<unsigned short *>("MODL", 2);
		auto MODLCHAR = race.Get<char *>("MODL", 2);

		subrecord *modl = race.GetField("MODL", 2);

		printf("modl size %u str %s offset %u", modl->hed->type, (char *)modl->data, modl->offset);

		cassert(ANAM, "no actor race anam sub");

		struct rc *rc = bsa_find(get_archives()[0], model);
		bsa_read(rc);
		struct nifp *character = malloc_nifp();
		character->path = model;
		character->buf = rc->buf;
		nifp_read(character);
		nifp_save((void *)model, character);

		Mesh *mesh = new Mesh();
		mesh->Construct(character);
		SkinnedMesh *sm = new SkinnedMesh(mesh);
		sm->Construct();

		skelly = new Skeleton();
		Animation *attack = new Animation(draugrAttack);
		attack->skeleton = skelly;
		skelly->animation = attack;

		skelly->Load(ANAM);
		skelly->Construct();

		auto ref = dungeon->refEditorIDs.find("gloomdraugrskelly");

		if (ref != dungeon->refEditorIDs.end())
		{
			Group *group = new Group();
			group->Add(skelly->baseBone->group);
			group->Add(mesh->baseGroup);
			printf("make skelly drawgroup!\n");
			drawGroup = new DrawGroup(group, ref->second->matrix);

			scene->Add(drawGroup);
		}
	}

	void Actor::Step()
	{
		if (skelly)
			skelly->Step();
		const float merry = 0.002;
		//if (drawGroup)
			//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace gloom