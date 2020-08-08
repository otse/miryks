#include "camera"

#include "dark2.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


Camera::Camera()
{
	pos = vec3(0);
	eye = vec3(0);
}

void Camera::Mouse(float x, float y)
{
	const float sens = .001f;

	fyaw += x * sens;
	fpitch -= y * sens;
}

void Camera::Call()
{
	while (fyaw > 2 * pif)
		fyaw -= 2 * pif;
	while (fyaw < 0)
		fyaw += 2 * pif;

	fpitch = fmaxf(-pif, fminf(0, fpitch));

	view = mat4(1.0f);
	view = rotate(view, fpitch, vec3(1, 0, 0));
	view = rotate(view, fyaw, vec3(0, 0, 1));
	view = translate(view, -pos - eye);

	using namespace dark2;

	float aspect = (float) width / (float) height;

	projection = perspective(
		radians(fzoom),
		aspect,
		0.1f,
		10000.0f);

	//log_("view ", glm::to_string(view));
}

void Camera::UpDown(const int dir, float time)
{
	return;
	float speed = 250 * time;
	if (shift)
		speed /= 10;

	if (r)
		pos.z += speed;
	if (f)
		pos.z -= speed;
}

void Camera::Move(float time)
{
	auto forward = [&](float n) {
		pos.x += n * sin(fyaw);
		pos.y += n * cos(fyaw);
	};
	
	auto strafe = [&](float n) {
		pos.x += n * cos(-fyaw);
		pos.y += n * sin(-fyaw);
	};

	float speed = 500 * time;

	if (shift)
		speed /= 10;

	if (w && !s)
		forward(speed);
	if (s && !w)
		forward(-speed / 2);

	if (a && !d)
		strafe(-speed);
	if (d && !a)
		strafe(speed);
}