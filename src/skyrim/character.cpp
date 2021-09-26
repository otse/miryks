#include <skyrim/actors.h>
#include <skyrim/interior.h>

#include <renderer/drawgroup.h>

namespace skyrim {
    Char::Char(const char *raceId)
	{
		anim = nullptr;
		race = get_race(raceId);
		skel = new Skel(race);
		hat = head = body = hands = feet = nullptr;
		const bool beggar = true;
		if (beggar)
		{
			hat = new SkinnedMesh("clothes\\beggarclothes\\hatm_0.nif");
			//head = new BodyPart("actors\\character\\character assets\\malehead.nif");
			body = new SkinnedMesh("clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new SkinnedMesh("clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new SkinnedMesh("clothes\\prisoner\\prisonershoes_0.nif");
		}
		hat->skel = skel;
		body->skel = skel;
		hands->skel = skel;
		feet->skel = skel;
		/*
			//head = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardhat_0.nif");
			//body = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greyboardrobe_0.nif");
			//feet = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardboots_0.nif");
		*/
		drawGroup = new DrawGroup(nullptr, mat4(1.0));
		if (hat)
			drawGroup->Add(hat->drawGroup);
		if (head)
			drawGroup->Add(head->drawGroup);
		if (body)
			drawGroup->Add(body->drawGroup);
		if (hands)
			drawGroup->Add(hands->drawGroup);
		if (feet)
			drawGroup->Add(feet->drawGroup);
		drawGroup->Update();
	};

	void Char::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref == dungeon->edIds.end())
			return;
		printf("place at %s\n", q);
		drawGroup->matrix = ref->second->matrix;
		drawGroup->Update();
		sceneDef->bigGroup->Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDef->Add(mirror);
	}

	void Char::SetAnimation(const char *path)
	{
		anim = new Anim(get_keyframes(path));
		anim->skel = skel;
		skel->anim = anim;
	}

	void Char::Step()
	{
		if (skel)
			skel->Step();
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
		//{
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			//drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		//}
	}
#if 0
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
#endif
}