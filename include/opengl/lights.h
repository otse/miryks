#ifndef OPENGL_LIGHTS_H
#define OPENGL_LIGHTS_H

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Group.h>

struct Light
{
	typedef LightShadow Shadow;
	Light();
	Shadow *shadow;
	mat4 matrix;
	mat4 matrixWorld;
	vec3 color;
	float intensity;
	float CalcDist() const;
	void Calc();
};
struct PointLight : Light
{
	PointLight();
	float distance, decay;
};
struct SpotLight : Light
{
	SpotLight();
	float distance, angle, penumbra, decay;
};
struct DirectionalLight : Light
{
	DirectionalLight();
	vec3 target;
};

#endif