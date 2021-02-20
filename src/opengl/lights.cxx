#include <OpenGL/Lights.h>

// https://community.khronos.org/t/uniform-buffer-objects-dynamic-sized-arrays-and-lights/70415

const float PI = pi<float>();
const float PI2 = pi<float>() * 2.f;

Light::Light() {
	matrix = mat4(1.0f);
	color = vec3(1);
	intensity = 1.f;
}

PointLight::PointLight() {
	distance = 100.f;
	decay = 1.0f;
}

SpotLight::SpotLight() {
	distance = 100.f;
	decay = 1.0f;
}

DirectionalLight::DirectionalLight() {
	target = vec3(0);
}

float Light::CalcDist() const
{
	return glm::distance(vec3(matrix[3]), camera->pos);
}

void Light::Calc() // Unused
{
	vec3 position, color;
	
	position = vec3(matrix[3]) * mat3(inverse(camera->view));
	position += vec3(camera->view[3]);
	color = color * intensity;
}