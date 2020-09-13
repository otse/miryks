#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <opengl/types.h>

#include <opengl/camera.h>
#include <opengl/scene.h>

#include "files.h"

#include <glad/glad.h>

void SetShaderSources();

extern ssrc simple, basic, fxs, postquad;

struct ShaderSettings
{
	bool diffuseMaps = true;
	bool normalMaps = true;
	bool specularMaps = true;
	bool dust = true;
};
extern ShaderSettings commonSettings;

struct Shader
{
	std::string CommonHeader();
	
	static std::map<std::string, Shader *> shaders;

	static Shader *active;

	bool dust = true;

	ssrc *const src;
	GLuint id;

	string header;

	Shader(ssrc *);

	void Compile();

	void Use();

	void SetInt(const char *, int) const;
	void SetBool(const char *, bool) const;
	void SetFloat(const char *, float) const;
	void SetVec2(const char *, const vec2 &) const;
	void SetVec3(const char *, const vec3 &) const;
	void SetVec4(const char *, const vec4 &) const;
	void SetMat2(const char *, const mat2 &) const;
	void SetMat3(const char *, const mat3 &) const;
	void SetMat4(const char *, const mat4 &) const;

private:
	void checkCompileErrors(GLuint, string);
};
#endif