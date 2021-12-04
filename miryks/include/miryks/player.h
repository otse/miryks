#pragma once

#include <miryks/actors.h>

namespace miryks
{
    class Player : public Char
	{
	public:
		ViewerCamera *thirdPersonCamera;
		float yaw;
		bool thirdPerson;
		Player();
		void toggleView();
		void Step();
		void Move();
	};
}