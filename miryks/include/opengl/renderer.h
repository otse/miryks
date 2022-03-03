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
#include <glm/gtx/string_cast.hpp>

using namespace glm;

const float pif = pi<float>();

typedef char *ShaderSource[3];

struct Aabb;
struct ShadowMapRenderer;

typedef Aabb AABB;

struct Light;
struct LightShadow;
struct DirectionalLight;
struct DirectionalLightShadow;
struct pointlight;
struct PointLightShadow;
struct spotlight;
struct SpotLightShadow;

struct List;
struct Group;
struct GroupBounded;
struct DrawGroup;
struct DrawGroupFlatSorted;

struct Texture;
struct Geometry;
struct Material;
struct MaterialPhong;
struct Shader;
struct Program;
struct RenderTarget;

struct Scene;
struct Camera;
struct FirstPersonCamera;
struct ViewerCamera;

typedef DrawGroup gl_drawgroup;
typedef Group gl_group;
typedef Shader gl_shader;
typedef Material gl_material;
typedef Geometry gl_geometry;
typedef Program gl_program;
typedef Scene gl_scene;
typedef Camera gl_camera;
typedef FirstPersonCamera gl_first_person_camera;

extern Camera *cameraCur;
extern Scene *sceneCur, *sceneDef;
extern FirstPersonCamera *personCam;
extern ViewerCamera *viewerCam;

extern RenderTarget *renderTargetDef;

void detectGlfwError();
void detectOpenGLError(const std::string = "?");

void overlay_renderer();
void renderer_init();
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
	float drawDistance = 1000.f;
};
extern RenderSettings renderSettings;

extern int width, height;
extern int fps;
extern float delta;

inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }

#endif