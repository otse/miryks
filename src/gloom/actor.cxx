#include <skyrim_units>
#include <libs>

#include <Gloom/Gloom.h>
#include <Gloom/Actor.h>
#include <Gloom/Collision.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <Gloom/Files.h>
#include <Gloom/Skeleton.h>
#include <Gloom/Interior.h>
#include <Gloom/Mesh.h>

#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Camera.h>

// good reads
// https://www.afkmods.com/index.php?/topic/5494-skyrim-lesse-skin-partitions-nif-to-ck/

namespace gloom
{
	Keyframes *draugrAttack = nullptr;

	Keyframes *humanIdle = nullptr;
	Keyframes *humanForward = nullptr;
	Keyframes *humanLeft = nullptr;
	Keyframes *humanRight = nullptr;
	Keyframes *humanBack = nullptr;

	Keyframes *GetKeyframes(const char *path)
	{
		Nif *nif = malloc_nifp();
		nif->path = path;
		char *lvalue = (char *)nif->buf;
		fbuf(path, &lvalue);
		nifp_read(nif);
		nifp_save(nif, nif);
		Kf *kf = new Kf(nif);
		return kf;
	}

	Record *GetRace(const char *raceId)
	{
		Record *race = nullptr;
		ObjectArray array;
		array(esp_top_grup(get_plugins()[0], "RACE")).forEach([&](unsigned int &i)
															  {
																  Record *record = array.getRecord(i);
																  auto editorId = getEditorIdOnly(record);
																  if (strcmp(editorId, raceId) == 0)
																  {
																	  race = record;
																	  array.stop = true;
																  }
															  });
		cassert(array.stop, "No such raceId !");
		return race;
	}

	BodyPart::BodyPart(const char *raceId, const char *model)
	{
		Object race = Object(GetRace(raceId));
		ExportRaceHkxToKf(raceId);
		auto anam = race.data<char *>("ANAM", 0);
		Rc *rc = load_rc("meshes\\", model, 0x1);
		Nif *character = load_nif(rc, false);
		skeleton = new Skeleton(anam);
		mesh = new Mesh(character);
		smesh = new SkinnedMesh(mesh, skeleton);
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

	void BodyPart::PutDown(const char *q)
	{
		auto ref = dungeon->editorIds.find(q);
		if (ref != dungeon->editorIds.end())
		{
			Group *group = new Group();
			group->Add(skeleton->baseBone->group);
			group->Add(mesh->baseGroup);
			//printf("make smesh->skeleton drawGroup!\n");
			drawGroup = new DrawGroup(group, ref->second->matrix);
			sceneDefault->drawGroups.Add(drawGroup);
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
		sceneDefault->drawGroups.Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDefault->Add(mirror);
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
		printf(" Player() \n");
		// We take over with custom movement
		Camera::DISABLE_MOVEMENT = true;
#if NO_HUMAN_PLAYER
		human = new Human();
		//human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4());
		drawGroup->group->visible = false;
		sceneDefault->drawGroups.Add(drawGroup);
//cameraCurrent->group->Add(human->group);
//fpc = new FirstPersonCamera;
#endif
		pose = vec3(cameraCurrent->pos);

		thirdPersonCamera = new ViewerCamera;
	}

	void Player::step()
	{
		move();
		cameraCurrent->pos = vec3(pose);
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
		//if (!dynamic_cast<FirstPersonCamera *>(cameraCurrent))
		//	return;
		vec3 down = vec3(0, 0, SU_TO_CM(-150));
		drawGroup->matrix = glm::translate(mat4(1.0), down + pose);
		drawGroup->matrix = rotate(drawGroup->matrix, -cameraCurrent->yaw, vec3(0, 0, 1));
		drawGroup->Reset();
#endif
	}

	void Player::toggleView()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			cameraCurrent = thirdPersonCamera;
#if NO_HUMAN_PLAYER
			drawGroup->group->visible = true;
#endif
			thirdPersonCamera->pos = pose;
			thirdPersonCamera->yaw = firstPersonCamera->yaw;
			thirdPersonCamera->pitch = firstPersonCamera->pitch;
			thirdPersonCamera->radius = 200;
		}
		else
		{
			cameraCurrent = firstPersonCamera;
#if NO_HUMAN_PLAYER
			drawGroup->group->visible = false;
#endif
			firstPersonCamera->pos = pose;
			firstPersonCamera->yaw = thirdPersonCamera->yaw;
			firstPersonCamera->pitch = thirdPersonCamera->pitch;
		}
	}

	void Player::move()
	{
		using namespace MyKeys;

		yaw = cameraCurrent->yaw;

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

		float speed = 200.f * gloom::delta;

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

} // namespace gloom