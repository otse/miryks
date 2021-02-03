#include <libs>

#include <Gloom/Actor.h>

#include <gloom/files.h>
#include <gloom/skeleton.h>
#include <gloom/level.h>
#include <gloom/mesh.h>

#include <opengl/group.h>
#include <opengl/scene.h>
#include <opengl/camera.h>

namespace gloom
{
	// Does a bunch of hxkcmd stuff. Export into a separate bsa inherent into the github?

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

	Actor::Actor(const char *raceId, const char *model)
	{
		//printf("actor of race %s\n", raceId);

		Object race = GetRace(raceId);

		ExportRaceHkxToKf(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.Get<char *>("ANAM", 0);

		auto MODL = race.Get<unsigned short *>("MODL", 2);
		auto MODLCHAR = race.Get<char *>("MODL", 2);

		Subrecord *modl = race.GetSubrecord("MODL", 2);

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
			//printf("make smesh->skeleton drawGroup!\n");
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
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//scene->Add(mirror);
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
		//human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4());
		scene->Add(drawGroup);
		//camera->group->Add(human->group);
	}

	void Player::Step()
	{
		
		human->Step();
		vec3 down = vec3(0, 0, -150 / ONE_SKYRIM_UNIT_IN_CM);
		drawGroup->matrix = glm::translate(mat4(1.0), down + first_person_camera->pos);
		drawGroup->matrix = rotate(drawGroup->matrix, -first_person_camera->fyaw, vec3(0, 0, 1));
		drawGroup->group->visible = false;

		
		if (!dynamic_cast<FirstPersonCamera *>(camera))
			return;
	}

} // namespace gloom