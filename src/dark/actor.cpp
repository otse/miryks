
#if 0
#include <skyrim_units>

#include <dark/dark.h>
#include <dark/actor.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>

#include <dark/files.h>
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
	Keyframes *humanIdle = nullptr;
	Keyframes *humanForward = nullptr;
	Keyframes *humanLeft = nullptr;
	Keyframes *humanRight = nullptr;
	Keyframes *humanBackward = nullptr;

	void load_animations()
	{
		draugrIdle = loadAnimDisk("anims/draugr/alcove_wake.kf");
		humanIdle = loadAnimDisk("anims/character/1hm_idle.kf");
		humanLeft = loadAnimDisk("anims/character/1hm_walkleft.kf");
		humanRight = loadAnimDisk("anims/character/1hm_walkright.kf");
		humanBackward = loadAnimDisk("anims/character/1hm_walkbackward.kf");
		humanForward = loadAnimDisk("anims/character/1hm_walkforward.kf");
	}

	Actor::Actor(const char *raceId, const char *model)
	{
		skinnedMesh = nullptr;
		skeleton = nullptr;
		animation = nullptr;
		drawGroup = nullptr;
		race = skyrim_get_race(raceId);
		auto anam = race.data<char *>("ANAM", 0);
		printf("bodypart raceid %s anam %s\n", raceId, anam);
		Res *rc = load_res(model);
		Nif *character = load_model(rc);
		skeleton = new Skeleton(anam);
		skinnedMesh = new SkinnedMesh(character, skeleton);
		if (raceId == "DraugrRace")
		{
			//if (!draugrAttack)
			//draugrAttack = loadAnimDisk("temp/draugr/kf/1hmattackf.kf");
			//draugrIdle = loadAnimDisk("anims/draugr/1hmidle.kf");
			animation = new Animation(draugrIdle);
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

	void Actor::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref != dungeon->edIds.end())
		{
			drawGroup = new DrawGroup(
				skinnedMesh->baseGroup, ref->second->matrix);
			//drawGroup->matrix = scale(drawGroup->matrix, vec3(1, 1, .5));
			sceneDef->bigGroup->Add(drawGroup);
		}
		else
			printf("actor:place noref %s\n", q);
	}

	void Actor::Step()
	{
		if (skinnedMesh)
			skinnedMesh->Forward();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

	// a human is either a beggar, or a greybeard
	// right now a greybeard will crash the game
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
			//head = new BodyPart("ImperialRace", "actors\\character\\character assets\\malehead.nif");
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
	};

	void Human::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref == dungeon->edIds.end())
			return;
		drawGroup->matrix = ref->second->matrix;
		sceneDef->bigGroup->Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDef->Add(mirror);
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
		//if (csphere)
		{
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			//drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		}
	}

	Player::Player()
	{
		//printf(" Player() \n");
		// We take over with custom movement
		personCam->disabled = true;
		human = new Human();
		//human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4(1.0));
		drawGroup->target->visible = false;
		sceneDef->bigGroup->Add(drawGroup);
//cameraCur->group->Add(human->group);
//fpc = new FirstPersonCamera;
		pose = vec3(personCam->pos);

		thirdPersonCamera = new ViewerCamera;
	}

	void Player::Step()
	{
		move();
		cameraCur->pos = vec3(pose);
		if (holding("w"))
		{
		}
		else
		{
			//human->
		}
		human->Step();
		//if (!dynamic_cast<FirstPersonCamera *>(cameraCur))
		//	return;
		vec3 down = vec3(0, 0, SU_TO_CM(-150));
		drawGroup->matrix = glm::translate(mat4(1.0), down + pose);
		drawGroup->matrix = rotate(drawGroup->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		drawGroup->Reset();
	}

	void Player::toggleView()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			cameraCur = thirdPersonCamera;
			drawGroup->target->visible = true;
			thirdPersonCamera->pos = pose;
			thirdPersonCamera->yaw = personCam->yaw;
			thirdPersonCamera->pitch = personCam->pitch;
			thirdPersonCamera->radius = 200;
		}
		else
		{
			cameraCur = personCam;
			drawGroup->target->visible = false;
			personCam->pos = pose;
			personCam->yaw = thirdPersonCamera->yaw;
			personCam->pitch = thirdPersonCamera->pitch;
		}
	}

	void Player::move()
	{
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

		if (!holding("lshift"))
		speed /= 10;

		if (holding("w") && !holding("s")) {
			forward(speed);
			//human
		}
		if (holding("s") && !holding("w")) {
			forward(-speed / 2);
		}

		if (holding("a") && !holding("d")) {
			strafe(-speed);
		}
		if (holding("d") && !holding("a")) {
			strafe(speed);
		}

		if (holding("r"))
			pose.z += speed / 2;
		if (holding("f"))
			pose.z -= speed / 2;
	}

} // namespace dark
#endif