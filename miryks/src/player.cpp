#include <miryks/player.h>

#include <opengl/renderer.h>
#include <opengl/camera.h>
#include <ogl/scene_graph.hpp>

namespace miryks
{
	Player::Player() : Char()
	{
		yaw = 0;
		thirdPerson = false;
		groupDrawer->visible = false;
		Place("gloomgenman");
		thirdPersonCamera = new ViewerCamera;
	}

	void Player::Step()
	{
		Move();
		Char::Step();

		if (pressing_key("v"))
		{
			printf("Toggle");
			toggleView();
		}

		// memory leak somewhere
		if (pressing_key("w"))
		{
			idle = false;
			//printf("set anim\n");
			SetAnim("anims/character/1hm_walkforward.kf");
		}
		else if (pressing_key("s"))
		{
			idle = false;
			SetAnim("anims/character/1hm_walkbackward.kf");
		}
		else if (pressing_key("a"))
		{
			idle = false;
			SetAnim("anims/character/1hm_walkleft.kf");
		}
		else if (pressing_key("d"))
		{
			idle = false;
			SetAnim("anims/character/1hm_walkright.kf");
		}
		else if (!holding_key("w") && !holding_key("s") && !holding_key("a") && !holding_key("d"))
		{
			if (!idle)
			{
				idle = true;
				SetAnim("anims/character/1hm_idle.kf");
			}
		}
		vec3 down = vec3(0, 0, -EYE_HEIGHT);
		groupDrawer->matrix = glm::translate(mat4(1.0), down + cameraCur->pos);
		groupDrawer->matrix = rotate(groupDrawer->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		groupDrawer->UpdateSideways();
		groupDrawer->Reset();
	}

	void Player::toggleView()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			groupDrawer->visible = true;
			thirdPersonCamera->pos = cameraCur->pos;
			thirdPersonCamera->yaw = personCam->yaw;
			thirdPersonCamera->pitch = personCam->pitch;
			thirdPersonCamera->radius = 200;
			cameraCur = thirdPersonCamera;
		}
		else
		{
			groupDrawer->visible = false;
			personCam->pos = cameraCur->pos;
			personCam->yaw = thirdPersonCamera->yaw;
			personCam->pitch = thirdPersonCamera->pitch;
			cameraCur = personCam;
		}
	}

	void Player::Move()
	{
	}
}