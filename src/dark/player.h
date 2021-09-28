#pragma once

#include <skyrim/actors.h>

namespace dark
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