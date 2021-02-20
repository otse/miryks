#ifndef OPENGL_LIGHTS_H
#define OPENGL_LIGHTS_H

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Group.h>

struct ShadowMapRenderer;

struct Light;
struct LightShadow;
struct DirectionalLight;
struct DirectionalLightShadow;
struct PointLight;
struct PointLightShadow;

struct Light
{
	typedef LightShadow Shadow;

	Shadow *shadow;
	mat4 matrix;
	mat4 matrixWorld;
};

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