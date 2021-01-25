#include <gloom/libs>

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
	Object GetRace(const char *raceId)
	{
		Grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

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

		Subrecord *modl = race.GetField("MODL", 2);

		//printf("modl size %u str %s offset %u", modl->hed->type, (char *)modl->data, modl->offset);

		cassert(ANAM, "no actor race anam sub");

		Rc *rc = bsa_find(get_archives()[0], model);
		bsa_read(rc);
		Nifp *character = malloc_nifp();
		character->path = model;
		character->buf = rc->buf;
		nifp_read(character);
		nifp_save((void *)model, character);

		skeleton = new Skeleton();
		skeleton->Load(ANAM);
		skeleton->Construct();
		skeleton->baseBone->group->visible = false;

		mesh = new Mesh();
		mesh->Construct(character);

		smesh = new SkinnedMesh();
		smesh->mesh = mesh;
		smesh->skeleton = skeleton;
		smesh->Construct();

		if (raceId == "DraugrRace")
		{
			animation = new Animation(draugrAttack);
			animation->skeleton = skeleton;
			skeleton->animation = animation;
		}

		if (raceId == "ImperialRace")
		{
			animation = new Animation(humanIdle);
			animation->skeleton = skeleton;
			skeleton->animation = animation;
		}
	}

	void Actor::PutDown(const char *q)
	{
		auto ref = dungeon->refEditorIDs.find(q);

		if (ref != dungeon->refEditorIDs.end())
		{
			Group *group = new Group();
			group->Add(skeleton->baseBone->group);
			group->Add(mesh->baseGroup);
			printf("make smesh->skeleton drawGroup!\n");
			drawGroup = new DrawGroup(group, ref->second->matrix);
			scene->Add(drawGroup);
		}
		else
		{
			printf("actor put down cant find %s\n", q);
		}
	}

	void Actor::Step()
	{
		if (smesh)
			smesh->Forward();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

	Human::Human()
	{
		head = new Actor("ImperialRace", "meshes\\actors\\character\\character assets\\malehead.nif");
		body = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonerclothes_0.nif");
		hands = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonercuffs_0.nif");
		feet = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonershoes_0.nif");

		group = new Group;
		//group->matrix = glm::translate(mat4(1), vec3(0, 0, 200));
		group->Add(head->mesh->baseGroup);
		group->Add(body->mesh->baseGroup);
		group->Add(hands->mesh->baseGroup);
		group->Add(feet->mesh->baseGroup);

		drawGroup = new DrawGroup(group, mat4());
	};

	void Human::Place(const char *q)
	{
		auto ref = dungeon->refEditorIDs.find(q);
		if (ref == dungeon->refEditorIDs.end())
			return;
		drawGroup->matrix = ref->second->matrix;
		scene->Add(drawGroup);
		DrawGroup *test = new DrawGroup(group, mat4());
		test->matrix = drawGroup->matrix;
		test->matrix = glm::translate(test->matrix, vec3(50, 0, 0));
		scene->Add(test);
	}

	void Human::Step()
	{
		head->Step();
		body->Step();
		hands->Step();
		feet->Step();
	}

	Player::Player()
	{
		human = new Human();
		human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4());
		scene->Add(drawGroup);
		//camera->group->Add(human->group);
	}

	void Player::Step()
	{
		human->Step();
		//drawGroup->matrix = glm::translate(human->drawGroup->matrix, vec3(0, 50, 0));
		drawGroup->matrix = glm::translate(mat4(1.0), first_person_camera->pos);
		if (!dynamic_cast<FirstPersonCamera *>(camera))
			return;
	}

} // namespace gloom