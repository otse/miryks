#include <libs>

#include <Gloom/Actor.h>
#include <Gloom/Collision.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <Gloom/Files.h>
#include <Gloom/Skeleton.h>
#include <Gloom/Interior.h>
#include <Gloom/Mesh.h>

#include <opengl/group.h>
#include <opengl/scene.h>
#include <opengl/camera.h>

namespace gloom
{
	Record *GetRace(const char *raceId)
	{
		Grup *top = esp_get_top_grup(get_plugins()[0], "RACE");

		cassert(top, "no race top grup");

		Record *race = nullptr;

		bool stop = false;

		Objects(top).ForEach(0, stop, [&](Objects &oa, size_t &i) {
			Record *record = oa.GetRecord(i);
			auto editorId = GetEditorIdOnly(record);
			if (strcmp(editorId, raceId) == 0)
			{
				race = record;
				stop = true;
			}
		});

		cassert(stop, "No such raceId !");

		return race;
	}

	Actor::Actor(const char *raceId, const char *model)
	{
		//printf("actor of race %s\n", raceId);

		Object race = Object(GetRace(raceId));

		ExportRaceHkxToKf(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.Data<char *>("ANAM", 0);

		auto MODL = race.Data<unsigned short *>("MODL", 2);
		auto MODLCHAR = race.Data<char *>("MODL", 2);

		Subrecord *modl = race.EqualRange("MODL", 2);

		//printf("modl size %u str %s offset %u", modl->hed->type, (char *)modl->data, modl->offset);

		cassert(ANAM, "no actor race anam sub");

		Rc *rc = bsa_find(get_archives()[0], model);
		bsa_read(rc);
		Nif *character = malloc_nifp();
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
		auto ref = dungeon->editorIds.find(q);

		if (ref != dungeon->editorIds.end())
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
		hat = head = body = hands = feet = nullptr;

		const bool beggar = true;
		const bool greybeard = false;
		if (beggar)
		{
			hat = new Actor("ImperialRace", "meshes\\clothes\\beggarclothes\\hatm_0.nif");
			head = new Actor("ImperialRace", "meshes\\actors\\character\\character assets\\malehead.nif");
			body = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonershoes_0.nif");
		}
		if (greybeard)
		{
			//head = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greybeardhat_0.nif");
			//body = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greyboardrobe_0.nif");
			//feet = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greybeardboots_0.nif");
		}

		group = new Group;
		//group->matrix = glm::translate(mat4(1), vec3(0, 0, 200));
		if (hat)
			group->Add(hat->mesh->baseGroup);
		if (head)
			group->Add(head->mesh->baseGroup);
		if (body)
			group->Add(body->mesh->baseGroup);
		if (hands)
			group->Add(hands->mesh->baseGroup);
		if (feet)
			group->Add(feet->mesh->baseGroup);

		drawGroup = new DrawGroup(group, mat4());

		csphere = nullptr;
	};

	void Human::Place(const char *q)
	{
		auto ref = dungeon->editorIds.find(q);
		if (ref == dungeon->editorIds.end())
			return;
		drawGroup->matrix = ref->second->matrix;
		csphere = new CSphere(vec3(drawGroup->matrix[3]) /*+vec3(0, 0, 1)*/);
		scene->Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//scene->Add(mirror);
	}

	void Human::Step()
	{
		if (hat)
			hat->Step();
		if (head)
			head->Step();
		if (body)
			body->Step();
		if (hands)
			hands->Step();
		if (feet)
			feet->Step();
		if (csphere)
		{
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		}
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