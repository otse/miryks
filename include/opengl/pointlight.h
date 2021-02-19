#ifndef OPENGL_POINTLIGHT_H
#define OPENGL_POINTLIGHT_H

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Group.h>

struct PointLight
{
	mat4 matrix = mat4(1.0);

	vec3 color = vec3(0, 0, 1);
	float distance = 500 * 1.428;
	float decay = 1.2f;
	float intensity = 1.f;

	vec3 flick;
	vec4 axes;
	bool flicker = false;

	vec3 aim;

	vec3 position_;
	vec3 color_;

	float GetDist() const;

	void Calc();

	//P
};

#endif