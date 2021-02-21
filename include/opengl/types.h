#ifndef OPENGL_TYPES_H
#define OPENGL_TYPES_H

// part of gloom

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/string_cast.hpp>

using namespace glm;

const float pif = pi<float>();

typedef char *ShaderSource[3];

struct Aabb;
struct ShadowMapRenderer;

struct Light;
struct LightShadow;
struct DirectionalLight;
struct DirectionalLightShadow;
struct PointLight;
struct PointLightShadow;
struct SpotLight;
struct SpotLightShadow;

struct List;
struct Group;
struct DrawGroup;

struct Texture;
struct Geometry;
struct Material;
struct Shader;
struct Program;
struct RenderTarget;

struct Scene;
struct Camera;
struct FirstPersonCamera;
struct ViewerCamera;

extern Camera *camera;
extern Scene *scene;

void detectGlfwError();
void detectOpenGLError(const std::string = "?");

void opengl_gui();
void openglScene();
void OGLReloadShaderSource();

struct RenderSettings
{
	bool diffuseMaps = true;
	bool normalMaps = true;
	bool specularMaps = true;
	bool glowMaps = true;
	bool dust = true;
	bool axes = false;
	bool AABBS = false;
	bool OBBS = false;
	float maximumBoundingVolume = 512000.f;
	float restRatio = 0.0f;
};
extern RenderSettings renderSettings;

#endif