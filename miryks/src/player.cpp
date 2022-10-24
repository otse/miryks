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
		jaw = nullptr;
		idling = false;
		footstep1 = new sound("sound\\fx\\npc\\draugr\\foot\\walk\\l\\npc_draugr_foot_walk_01.wav");
		footstep2 = new sound("sound\\fx\\npc\\draugr\\foot\\walk\\l\\npc_draugr_foot_walk_02.wav");
		footstep3 = new sound("sound\\fx\\npc\\draugr\\foot\\walk\\r\\npc_draugr_foot_walk_03.wav");
		footstep4 = new sound("sound\\fx\\npc\\draugr\\foot\\walk\\r\\npc_draugr_foot_walk_04.wav");
		auto has = skel->bonesNamed.find("NPC Head [Jaw]");
		if (has != skel->bonesNamed.end())
			jaw = has->second;
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

		groupDrawer->matrix = glm::translate(mat4(1.0), cameraCur->pos);
		capsule = new collision::capsule;
		capsule->make(vec3(groupDrawer->matrix[3]) + vec3(0, 0, capsule->half * 2 + capsule->height / 2));
		groupDrawer->Add(new GroupDrawer(skel->root, mat4(1.0)));
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);

		//Place("gloomgenman");
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
		// keyf->loop = true;
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

		idling = false;

		if (holding_key("w") && holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hforwardleft.kf");
		}
		else if (holding_key("w") && holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hforwardright.kf");
		}
		else if (holding_key("w") && !holding_key("s"))
		{
			SetAnim("anims/draugr/_h2hforward.kf");
		}
		else if (holding_key("s") && holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hbackwardleft.kf");
		}
		else if (holding_key("s") && holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hbackwardright.kf");
		}
		else if (holding_key("s") && !holding_key("w"))
		{
			SetAnim("anims/draugr/_h2hbackward.kf");
		}
		else if (holding_key("a") && !holding_key("d"))
		{
			SetAnim("anims/draugr/_h2hleft.kf");
		}
		else if (holding_key("d") && !holding_key("a"))
		{
			SetAnim("anims/draugr/_h2hright.kf");
		}
		else
		{
			idling = true;
			SetAnim("anims/draugr/_h2hidle.kf");
		}

		if (!idling)
		{
			if (anim->time > 0.6 && anim->time < 0.7 && !footstep1->is_playing() && !footstep2->is_playing())
			{
				if (((float)rand() / RAND_MAX) < 0.5)
				{
					if (!footstep1->is_playing())
						footstep1->play();
				}
				else
				{
					if (!footstep2->is_playing())
					{
						footstep2->play();
					}
				}
			}

			if (anim->time > 0.1 && anim->time < 0.2 && !footstep3->is_playing() && !footstep4->is_playing())
			{
				if (((float)rand() / RAND_MAX) < 0.5)
				{
					if (!footstep3->is_playing())
						footstep3->play();
				}
				else
				{
					if (!footstep4->is_playing())
						footstep4->play();
				}
			}
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

		// btVector3 dir = btVector3(0, 0, -10);
		// capsule->rigidBody->applyCentralImpulse(dir);

		float force = 17;
		// btVector3 dir = btVector3(0.f, 0.f, 0.f);

		float x = 0;
		float y = 0;

		if (holding_key("w") && !holding_key("s"))
		{
			y = 1;
		}
		if (holding_key("s") && !holding_key("w"))
		{
			y = -1;
		}
		if (holding_key("d") && !holding_key("a"))
		{
			x = 1;
		}
		if (holding_key("a") && !holding_key("d"))
		{
			x = -1;
		}
		if (pressing_key("space"))
		{
			btVector3 dir = btVector3(0, 0, 500);
			// capsule->rigidBody->setLinearVelocity(dir);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}

		if (x || y)
		{
			float angle = atan2(x, y);
			angle += cameraCur->yaw;

			btVector3 dir = btVector3(sin(angle) * force, cos(angle) * force, 0);
			capsule->rigidBody->applyCentralImpulse(dir);
			capsule->rigidBody->activate();
		}
		/*
		float angle = atan2(b[0], b[1]);
		this.angle = angle;
		x = speed * Math.sin(angle);
		y = speed * Math.cos(angle);
		*/

		// capsule->rigidBody->
		capsule->step();

		vec3 up = vec3(0, 0, 140 * ONE_CENTIMETER_IN_SKYRIM_UNITS);
		vec3 origin = collision::bt_to_glm(capsule->get_world_transform().getOrigin());
		vec3 camera = origin + up;
		origin = origin - vec3(0, 0, capsule->half * 2 + capsule->height / 2);
		groupDrawer->matrix = glm::translate(mat4(1.0), origin);
		cameraCur->pos = camera;

		mat4 matrix = glm::translate(mat4(1.0), origin);

		matrix = rotate(matrix, -cameraCur->yaw, vec3(0, 0, 1));
		matrix = glm::translate(matrix, vec3(0, 10, 0));
		matrix = matrix * jaw->matrixWorld;
		cameraCur->pos = vec3(matrix[3]);

		// groupDrawer->matrix[3] = vec4(collision::bt_to_glm(capsule->get_world_transform().getOrigin()), 1);
		// groupDrawer->matrix = glm::translate(groupDrawer->matrix, forward);
		groupDrawer->matrix = rotate(groupDrawer->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		// groupDrawer->matrix = rotate(groupDrawer->matrix, -(cameraCur->pitch + (pif / 2)), vec3(1, 0, 0));
		groupDrawer->UpdateSideways();
		/*groupDrawer->matrix = glm::translate(mat4(1.0), cameraCur->pos);
		groupDrawer->UpdateSideways();*/
		// groupDrawer->Reset();
	}

	void Player::Teleport()
	{
		printf("Teleport\n");
		capsule->set_position(cameraCur->pos);
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