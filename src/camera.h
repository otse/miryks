#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class camera_t
{
public:
	vec3 pos, eye;
	mat4 yaw, pitch, aim, view;
	mat4 projection;

	float fyaw = 0, fpitch = -glm::pi<float>() / 2;
	float fzoom = 45;
	bool w = false, a = false, s = false, d = false;
	bool r = false, f = false;
	bool shift = false;

	camera_t();

	void mouse(float, float);
	void move(float);
	void call();
	void up_down(const int, float);
};

#endif