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
		collision::orb *orb;
		Group *head;
		GroupDrawer *groupDrawer;
		SkinnedMesh *body, *body2, *hands;
		bool thirdPerson;
		bool idle;
		float yaw;
		Player();
		void Step();
		void Move();
		void SetAnim(const char *);
		void Place(const char *);
		void Toggle();

		ViewerCamera *thirdPersonCamera;
	};
}