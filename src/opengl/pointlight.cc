#include "pointlight"

// https://community.khronos.org/t/uniform-buffer-objects-dynamic-sized-arrays-and-lights/70415

const float PI = pi<float>();
const float PI2 = pi<float>() * 2.f;

float pointlight_t::GetDist() const
{
	return glm::distance(vec3(matrix[3]), camera->pos);
}

void pointlight_t::Calc()
{
	position_ = vec3(matrix[3]) * mat3(inverse(camera->view));
	position_ += vec3(camera->view[3]);
	color_ = color * intensity;
}