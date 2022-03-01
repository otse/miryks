#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

// part of gloom

#include <opengl/renderer.h>

#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
public:
	bool disabled;
	float yaw, pitch, fov;
	vec3 pos;
	mat4 view, projection;
	Group *group;
	DrawGroup *drawGroup;
	
	Camera();

	virtual void Mouse(float, float){};
	virtual void Update(float){};

	void Wasd(float);
	void SetProjection();
};

struct FirstPersonCamera : public Camera
{
	Group *hands;

	vec3 eye;

	FirstPersonCamera();
	virtual void Mouse(float, float);
	virtual void Update(float);
};

struct ViewerCamera : public Camera
{
	vec3 center;
	float radius, zoom;

	ViewerCamera();
	virtual void Mouse(float, float);
	virtual void Update(float);
};

#endif