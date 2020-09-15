#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <opengl/types.h>

#include <glm/gtc/matrix_transform.hpp>


struct Camera {
	Camera();

	vec3 pos;
	mat4 view, projection;
	float fzoom;
	bool disabled;

	virtual void Mouse(float, float) {};
	virtual void Update(float) {};

	void SetProjection();
};

struct FirstPersonCamera : public Camera
{
	vec3 eye;
	float fyaw = 0, fpitch = -pif / 2;
	bool w, a, s, d, r, f;
	bool shift;

	FirstPersonCamera();

	virtual void Mouse(float, float);
	virtual void Update(float);

	void Move(float);
};

struct ViewerCamera : public Camera
{
	vec3 center;
	float radius, yaw, pitch, zoom;

	ViewerCamera();

	virtual void Mouse(float, float);
	virtual void Update(float);
};

#endif