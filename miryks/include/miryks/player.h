#pragma once

#include <miryks/actors.h>

namespace miryks
{
    class Player : public Char
	{
	public:
		Player();
		ViewerCamera *thirdPersonCamera;
		bool thirdPerson;
		float yaw;
		void toggleView();
		void Step();
		void Move();
	};
}