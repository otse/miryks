#include <dark/player.h>

#include <renderer/renderer.h>
#include <renderer/camera.h>
#include <renderer/drawgroup.h>

namespace dark
{
	Player::Player() : Char()
	{
        yaw = 0;
        thirdPerson = false;
		drawGroup->visible = false;
		Place("gloomgenman");
		thirdPersonCamera = new ViewerCamera;
	}

	void Player::Step()
	{
        Move();
		Char::Step();
		if (pressing("w"))
		{
			SetAnim("anims/character/1hm_walkforward.kf");
		}
		else if (pressing("s"))
		{
			SetAnim("anims/character/1hm_walkbackward.kf");
		}
		else if (pressing("a"))
		{
			SetAnim("anims/character/1hm_walkleft.kf");
		}
		else if (pressing("d"))
		{
			SetAnim("anims/character/1hm_walkright.kf");
		}
		else if (!holding("w") && !holding("s") && !holding("a") && !holding("d"))
		{
			SetAnim("anims/character/1hm_idle.kf");
		}
		vec3 down = vec3(0, 0, -EYE_HEIGHT);
		drawGroup->matrix = glm::translate(mat4(1.0), down + cameraCur->pos);
		drawGroup->matrix = rotate(drawGroup->matrix, -cameraCur->yaw, vec3(0, 0, 1));
		drawGroup->Update();
		drawGroup->Reset();
	}

	void Player::toggleView()
	{
		thirdPerson = !thirdPerson;

		if (thirdPerson)
		{
			drawGroup->visible = true;
			thirdPersonCamera->pos = cameraCur->pos;
			thirdPersonCamera->yaw = personCam->yaw;
			thirdPersonCamera->pitch = personCam->pitch;
			thirdPersonCamera->radius = 200;
			cameraCur = thirdPersonCamera;
		}
		else
		{
			drawGroup->visible = false;
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