#pragma once

#include <skyrim/actors.h>

namespace dark
{
    class Player : public Char
	{
	public:
		ViewerCamera *thirdPersonCamera;
		vec3 pose;
		float yaw;
		bool thirdPerson;
		Player();
		void toggleView();
		void Step();
		void Move();
	};
}