#include <skyrim_units>


#include <dark/dark.h>
#include <dark/actor.h>
#include <dark/collision.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>

#include <core/files.h>
#include <skyrim/skeleton.h>
#include <skyrim/interior.h>
#include <skyrim/mesh.h>

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/scene.h>
#include <renderer/camera.h>

// good reads
// https://www.afkmods.com/index.php?/topic/5494-skyrim-lesse-skin-partitions-nif-to-ck/

using namespace skyrim;

namespace dark
{
	Keyframes *draugrAttack = nullptr;

	Keyframes *humanIdle = nullptr;
	Keyframes *humanForward = nullptr;
	Keyframes *humanLeft = nullptr;
	Keyframes *humanRight = nullptr;
	Keyframes *humanBack = nullptr;

	Keyframes *GetKeyframes(const char *path)
	{
		Nif *nif = calloc_nifp();
		nif->path = path;
		char *lvalue = (char *)nif->buf;
		fbuf(path, &lvalue);
		nif_read(nif);
		ext_nif_save(nif, nif);
		Kf *kf = new Kf(nif);
		return kf;
	}

	crecordp GetRace(const char *raceId)
	{
		crecordp race = nullptr;
		Grup array;
		grupp top = esp_top_grup(get_plugins()[0], "RACE");
		array(top).foreach([&](unsigned int &i) {
			Record object = array.get<record *>(i);
			auto edId = object.editorId();
			if (strcmp(edId, raceId) == 0)
			{
				race = object.rcd;
				return true;
			}
			return false;
		}, 0);
		assertc(race);
		return race;
	}

	BodyPart::BodyPart(const char *raceId, const char *model)
	{
		Record race = Record(GetRace(raceId));
		ExportRaceHkxToKf(raceId);
		auto anam = race.data<char *>("ANAM", 0);
		Rc *rc = load_rc("meshes\\", model, 0x1);
		Nif *character = import_nif(rc, false);
		skeleton = new Skeleton(anam);
		smesh = new SkinnedMesh(character, skeleton);
		if (raceId == "DraugrRace")
		{
			printf("animation!\n");
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

	void BodyPart::PutDown(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref != dungeon->edIds.end())
		{
			Group *group = new Group();
			//group->Add(skeleton->baseBone->group);
			group->Add(smesh->baseGroup);
			//printf("make smesh->skeleton drawGroup!\n");
			drawGroup = new DrawGroup(group, ref->second->matrix);
			sceneDef->drawGroups.Add(drawGroup);
		}
		else
		{
			printf("actor put down cant find %s\n", q);
		}
	}

	void BodyPart::step()
	{
		if (smesh)
			smesh->forward();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

	Human::Human()
	{
		//Object race = Object(GetRace("ImperialRace"));
		//ExportRaceHkxToKf("ImperialRace");
		//auto ANAM = race.data<char *>("ANAM", 0);
		hat = head = body = hands = feet = nullptr;
		const bool beggar = true;
		const bool greybeard = false;
		if (beggar)
		{
			hat = new BodyPart("ImperialRace", "clothes\\beggarclothes\\hatm_0.nif");
			head = new BodyPart("ImperialRace", "actors\\character\\character assets\\malehead.nif");
			body = new BodyPart("ImperialRace", "clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new BodyPart("ImperialRace", "clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new BodyPart("ImperialRace", "clothes\\prisoner\\prisonershoes_0.nif");
		}
		if (greybeard)
		{
			//head = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardhat_0.nif");
			//body = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greyboardrobe_0.nif");
			//feet = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardboots_0.nif");
		}
		group = new Group;
		//group->matrix = glm::translate(mat4(1), vec3(0, 0, 200));
		if (hat)
			group->Add(hat->smesh->baseGroup);
		if (head)
			group->Add(head->smesh->baseGroup);
		if (body)
			group->Add(body->smesh->baseGroup);
		if (hands)
			group->Add(hands->smesh->baseGroup);
		if (feet)
			group->Add(feet->smesh->baseGroup);
		drawGroup = new DrawGroup(group, mat4());
		csphere = nullptr;
	};

	void Human::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref == dungeon->edIds.end())
			return;
		drawGroup->matrix = ref->second->matrix;
		csphere = new CSphere(vec3(drawGroup->matrix[3]) /*+vec3(0, 0, 1)*/);
		sceneDef->drawGroups.Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDef->Add(mirror);
	}

	void Human::step()
	{
		if (hat)
			hat->step();
		if (head)
			head->step();
		if (body)
			body->step();
		if (hands)
			hands->step();
		if (feet)
			feet->step();
		if (csphere)
		{
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			//drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		}
	}

#define NO_HUMAN_PLAYER 1

	Player::Player()
	{
		//printf(" Player() \n");
		// We take over with custom movement
		Camera::DISABLE_MOVEMENT = true;
#if NO_HUMAN_PLAYER
		human = new Human();
		//human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4());
		drawGroup->group->visible = false;
		sceneDef->drawGroups.Add(drawGroup);
//cameraCur->group->Add(human->group);
//fpc = new FirstPersonCamera;
#endif
		pose = vec3(cameraCur->pos);

		thirdPersonCamera = new ViewerCamera;
	}

	void Player::step()
	{
		move();
		cameraCur->pos = vec3(pose);
		using namespace MyKeys;
		if (w)
		{
		}
		else
		{
			//human->
		}
#if NO_HUMAN_PLAYER
		human->step();
		//if (!dynamic_cast<FirstPersonCamera *>(cameraCur))
		//	return;
		vec3 down = vec3(0, 0, SU_TO_CM(-150));
		drawGroup->matrix = glm::translate(mat4(1.0), down + pose);
		drawGroup->matrix = rotate(drawGroup->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		drawGroup->Reset();
#endif
	}

	void Player::toggleView()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			cameraCur = thirdPersonCamera;
#if NO_HUMAN_PLAYER
			drawGroup->group->visible = true;
#endif
			thirdPersonCamera->pos = pose;
			thirdPersonCamera->yaw = personCam->yaw;
			thirdPersonCamera->pitch = personCam->pitch;
			thirdPersonCamera->radius = 200;
		}
		else
		{
			cameraCur = personCam;
#if NO_HUMAN_PLAYER
			drawGroup->group->visible = false;
#endif
			personCam->pos = pose;
			personCam->yaw = thirdPersonCamera->yaw;
			personCam->pitch = thirdPersonCamera->pitch;
		}
	}

	void Player::move()
	{
		using namespace MyKeys;

		yaw = cameraCur->yaw;

		auto forward = [&](float n)
		{
			pose.x += n * sin(yaw);
			pose.y += n * cos(yaw);
		};

		auto strafe = [&](float n)
		{
			pose.x += n * cos(-yaw);
			pose.y += n * sin(-yaw);
		};

		float speed = 200.f * delta;

		if (shift)
			speed /= 10;

		if (w && !s)
			forward(speed);
		if (s && !w)
			forward(-speed / 2);

		if (a && !d)
			strafe(-speed);
		if (d && !a)
			strafe(speed);

		if (r)
			pose.z += speed / 2;
		if (f)
			pose.z -= speed / 2;
	}

} // namespace dark