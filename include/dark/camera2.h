#ifndef GLOOM_CAMERA2_H
#define GLOOM_CAMERA2_H

// part of gloom

#include <opengl/Types.h>

#include <glm/gtc/matrix_transform.hpp>

struct Camera2 : public Camera
{
public:
	Camera2();
	Group *group;
	DrawGroup *drawGroup;
	vec3 pos;
	mat4 view;
	float fzoom;
	bool disabled;
	static double prev[2];
	virtual void Mouse(float, float){};
	virtual void Update(float){};
	void SetProjection();
};

#endif