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
		Place("gloomgenman");
		thirdPersonCamera = new ViewerCamera;
	}

	void Player::Step()
	{
        Move();
		Char::Step();
		if (holding("w"))
		{
		}
		else
		{
		}
		vec3 down = vec3(0, 0, 1.428f / -150);
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