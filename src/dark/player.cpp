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
		// take over with custom movement
		personCam->disabled = true;
		Place("gloomgenman");
//cameraCur->group->Add(human->group);
//fpc = new FirstPersonCamera;
		pose = vec3(personCam->pos);
		thirdPersonCamera = new ViewerCamera;
	}

	void Player::Step()
	{
		Char::Step();
        Move();
		cameraCur->pos = vec3(pose);
		if (holding("w"))
		{
		}
		else
		{
			//human->
		}
		//if (!dynamic_cast<FirstPersonCamera *>(cameraCur))
		//	return;
		vec3 down = vec3(0, 0, 1.428f * -150);
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

	void Player::Move()
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
}