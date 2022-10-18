#pragma once

#include <miryks/actors.h>
#include <miryks/collision_forwards.h>

namespace miryks
{
    class Player
	{
	public:
		record race;
		skeleton *skel;
		animation *anim;
		bone *jaw;
		collision::capsule *capsule;
		ModelSkinned *modelSkinned;
		GroupDrawer *groupDrawer;
		bool thirdPerson;
		float yaw;
		int walkstate = 0;
		Player();
		void Step();
		void Move();
		void SetAnim(const char *);
		void Place(const char *);
		void Teleport();
		void Toggle();

		ViewerCamera *thirdPersonCamera;
	};
}