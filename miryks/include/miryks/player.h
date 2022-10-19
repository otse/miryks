#pragma once

#include <miryks/actors.h>
#include <miryks/sound.h>
#include <miryks/collision_forwards.h>

namespace miryks
{
    class Player
	{
	public:
		record race;
		skeleton *skel;
		animation *anim;
		sound *footstep1, *footstep2, *footstep3, *footstep4;
		bone *jaw;
		ModelSkinned *modelSkinned;
		collision::capsule *capsule;
		Group *head;
		GroupDrawer *groupDrawer;
		SkinnedMesh *body, *body2, *hands;
		bool thirdPerson;
		bool idling;
		float yaw;
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