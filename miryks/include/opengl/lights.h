#ifndef OPENGL_LIGHTS_H
#define OPENGL_LIGHTS_H

// part of gloom

#include <opengl/renderer.h>

#include <opengl/camera.h>
#include <opengl/scene_graph.hpp>

struct Light
{
	typedef LightShadow Shadow;
	Light();
	Shadow *shadow;
	mat4 matrix;
	vec3 color;
	float intensity, distance, decay;
	float CalcDist() const;
	void Calc();
	
	vec3 target;
};
struct pointlight : Light
{
	pointlight();
};
struct spotlight : Light
{
	spotlight();
	float angle, penumbra;
	vec3 target;
};
struct DirectionalLight : Light
{
	DirectionalLight();
	vec3 target;
};

#endif