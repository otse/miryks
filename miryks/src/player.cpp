#include <miryks/player.h>
#include <miryks/collision.h>

#include <opengl/renderer.h>
#include <opengl/camera.h>
#include <opengl/scene_graph.hpp>

namespace miryks
{
	Player::Player()
	{
		yaw = 0;
		thirdPerson = false;
		race = dig_race("DraugrRace", 0);
		skel = new skeleton(race.data<char *>("ANAM"));
		modelSkinned = new ModelSkinned("actors\\draugr\\character assets\\draugrmale01.nif");
		capsule = nullptr;
		// skel->mixer = new animation_mixer(skel);
		printf("player skel anam %s\n", race.data<char *>("ANAM"));
		// skel = new skeleton("actors\\character\\_1stperson\\skeleton.nif");
		// thirdPerson = false;
		// groupDrawer->visible = false;
		// Place("gloomgenman");
		anim = nullptr;
		thirdPersonCamera = new ViewerCamera;

		// SetAnim("anims/character/1hm_1stp_run.kf");
		//  SetAnim("anims/character/1hm_walkforward.kf");

		groupDrawer = new GroupDrawer(modelSkinned->baseGroup, mat4(1.0));
		groupDrawer->name = "Player";

		groupDrawer->matrix = glm::translate(mat4(1.0), cameraCur->pos + vec3(0, 0, 100));
		capsule = new collision::capsule(groupDrawer);
		groupDrawer->Add(new GroupDrawer(skel->root, mat4(1.0)));
		groupDrawer->UpdateSideways();
		Place("gloomgenman");
	}

	void Player::Place(const char *q)
	{
		if (!ginterior)
			return;
		auto ref = ginterior->ids.find(q);
		if (ref == ginterior->ids.end())
			return;
		printf("place at %s\n", q);
		groupDrawer->matrix = ref->second->matrix;
		// groupDrawer->matrix = glm::translate(groupDrawer->matrix, vec3(50, 0, 0));
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);
		// Create an offsetted mirror of Man
		/*GroupDrawer *mirror = new GroupDrawer(groupDrawer, mat4());
		mirror->matrix = groupDrawer->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));
		sceneDef->bigGroup->Add(groupDrawer);*/
	}

	void Player::SetAnim(const char *path)
	{
		// if (anim)
		// 	delete anim;
		keyframes *keyf = get_keyframes(path);
		if (anim && anim->keyf == keyf)
			return;
		//keyf->loop = true;
		skel->SetFreeze();
		anim = new animation(keyf);
		anim->skel = skel;
		skel->anim = anim;
		// skel->mixer->SetFront(anim);
	}

	void Player::Step()
	{
		if (skel)
			skel->Step();
		
		if (modelSkinned)
			modelSkinned->Step(skel);

		if (pressing_key("v"))
		{
			Toggle();
		}

		if (holding_key("w") && holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hforwardleft.kf");
		}
		else if (holding_key("w") && holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hforwardright.kf");
		}
		else if (holding_key("w"))
		{
			SetAnim("anims/draugr/_h2hforward.kf");
		}
		else if (holding_key("s"))
		{
			SetAnim("anims/draugr/_h2hbackward.kf");
		}
		else if (holding_key("s") && holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hbackwardleft.kf");
		}
		else if (holding_key("s") && holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hbackwardright.kf");
		}
		else if (holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hleft.kf");
		}
		else if (holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hright.kf");
		}
		else
		{
			SetAnim("anims/draugr/_h2hidle.kf");
		}
		
		/*if (holding_key("w"))
		{
			// float x = cos(cameraCur->yaw) * 5;
			// float y = sin(cameraCur->yaw) * 5;
			btVector3 dir = btVector3(sin(cameraCur->yaw), cos(cameraCur->yaw), 0);
			capsule->rigidBody->setLinearVelocity(dir * 200.f);
			capsule->gravitate();
			//capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		if (holding_key("s"))
		{
			btVector3 dir = btVector3(sin(cameraCur->yaw), cos(cameraCur->yaw), 0);
			btVector3 lin_vel = capsule->rigidBody->getLinearVelocity();
			//dir.setY(-1000);
			capsule->rigidBody->setLinearVelocity(dir * -200.f);
			capsule->gravitate();
			capsule->rigidBody->activate();
		}
		if (holding_key("d"))
		{
			btVector3 dir = btVector3(cos(cameraCur->yaw), sin(-cameraCur->yaw), 0);
			capsule->rigidBody->setLinearVelocity(dir * 200.f);
			capsule->gravitate();
			capsule->rigidBody->activate();
		}
		if (holding_key("a"))
		{
			btVector3 dir = btVector3(cos(cameraCur->yaw), sin(-cameraCur->yaw), 0);
			capsule->rigidBody->setLinearVelocity(dir * -200.f);
			capsule->gravitate();
			capsule->rigidBody->activate();
		}
		if (pressing_key("space"))
		{
			btVector3 dir = btVector3(0, 0, 500);
			//capsule->rigidBody->setLinearVelocity(dir);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		
		capsule->step();
		*/

		//btVector3 dir = btVector3(0, 0, -10);
		//capsule->rigidBody->applyCentralImpulse(dir);


		float force = 20;

		if (holding_key("w"))
		{
			// float x = cos(cameraCur->yaw) * 5;
			// float y = sin(cameraCur->yaw) * 5;
			btVector3 dir = btVector3(sin(cameraCur->yaw) * force, cos(cameraCur->yaw) * force, 0);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		if (holding_key("s"))
		{
			btVector3 dir = btVector3(sin(cameraCur->yaw) * -force, cos(cameraCur->yaw) * -force, 0);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		if (holding_key("d"))
		{
			btVector3 dir = btVector3(cos(cameraCur->yaw) * force, sin(cameraCur->yaw) * -force, 0);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		if (holding_key("a"))
		{
			btVector3 dir = btVector3(cos(cameraCur->yaw) * -force, sin(cameraCur->yaw) * force, 0);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		if (pressing_key("space"))
		{
			btVector3 dir = btVector3(0, 0, 500);
			//capsule->rigidBody->setLinearVelocity(dir);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}

		//capsule->rigidBody->

		vec3 up = vec3(0, 0, 140 * ONE_CENTIMETER_IN_SKYRIM_UNITS);
		vec3 origin = collision::bt_to_glm(capsule->get_world_transform().getOrigin());
		vec3 camera = origin + up;
		origin = origin - vec3(0, 0, capsule->half);
		groupDrawer->matrix = glm::translate(mat4(1.0), origin);
		cameraCur->pos = camera;

		mat4 matrix = glm::translate(mat4(1.0), origin);
		auto has = skel->bonesNamed.find("NPC Head [Jaw]");
		if (has != skel->bonesNamed.end())
		{
			bone *bone = has->second;
			matrix = rotate(matrix, -cameraCur->yaw, vec3(0, 0, 1));
			matrix = glm::translate(matrix, vec3(0, 10, 0));
			matrix = matrix * bone->matrixWorld;
			cameraCur->pos = vec3(matrix[3]);
		}

		// groupDrawer->matrix[3] = vec4(collision::bt_to_glm(capsule->get_world_transform().getOrigin()), 1);
		// groupDrawer->matrix = glm::translate(groupDrawer->matrix, forward);
		groupDrawer->matrix = rotate(groupDrawer->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		//groupDrawer->matrix = rotate(groupDrawer->matrix, -(cameraCur->pitch + (pif / 2)), vec3(1, 0, 0));
		groupDrawer->UpdateSideways();
		/*groupDrawer->matrix = glm::translate(mat4(1.0), cameraCur->pos);
		groupDrawer->UpdateSideways();*/
		// groupDrawer->Reset();
	}

	void Player::Teleport()
	{
		capsule->set_position(collision::glm_to_bt(cameraCur->pos));
	}

	void Player::Move()
	{
	}

	void Player::Toggle()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			// groupDrawer->visible = true;
			thirdPersonCamera->pos = cameraCur->pos;
			thirdPersonCamera->yaw = personCam->yaw;
			thirdPersonCamera->pitch = personCam->pitch;
			thirdPersonCamera->radius = 200;
			cameraCur = thirdPersonCamera;
		}
		else
		{
			// groupDrawer->visible = false;
			personCam->pos = cameraCur->pos;
			personCam->yaw = thirdPersonCamera->yaw;
			personCam->pitch = thirdPersonCamera->pitch;
			cameraCur = personCam;
		}
	}
}