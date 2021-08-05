#include <renderer/shader.h>
#include <core/basefile.h>

extern "C"
{
#include "common.h"
}

Shader *Shader::active = nullptr;

//shader_t *shader_current;

#include <renderer/types.h>

#include <renderer/camera.h>
#include <renderer/scene.h>

#include <core/files.h>

#include <glad/glad.h>

Shader *basicShader;

ShaderSource simple, basic, fxs, postquad;

void SetShaderSources()
{
	simple[0] = (char *)"simple";
	basic[0] = (char *)"basic";
	fxs[0] = (char *)"fx";
#if 1
	fbuf("gl/simple.vert", &simple[1], true);
	fbuf("gl/simple.frag", &simple[2], true);
	fbuf("gl/basic.vert", &basic[1], true);
	fbuf("gl/basic.frag", &basic[2], true);
	fbuf("gl/fx.vert", &fxs[1], true);
	fbuf("gl/fx.frag", &fxs[2], true);
	fbuf("gl/post.vert", &postquad[1], true);
	fbuf("gl/post.frag", &postquad[2], true);
	printf("set shader sources\n");
#else
	static std::vector<std::string> sources;
	sources.push_back(basefile_offshore("basefile", "gl/simple.vert"));
	sources.push_back(basefile_offshore("basefile", "gl/simple.frag"));
	sources.push_back(basefile_offshore("basefile", "gl/basic.vert"));
	sources.push_back(basefile_offshore("basefile", "gl/basic.frag"));
	sources.push_back(basefile_offshore("basefile", "gl/post.vert"));
	sources.push_back(basefile_offshore("basefile", "gl/post.frag"));
	simple[1] = (char *)sources[0].c_str();
	simple[2] = (char *)sources[1].c_str();
	basic[1] = (char *)sources[2].c_str();
	basic[2] = (char *)sources[3].c_str();
	postquad[1] = (char *)sources[4].c_str();
	postquad[2] = (char *)sources[5].c_str();
#endif
}

std::map<std::string, Shader *> Shader::shaders;

Shader::Shader(ShaderSource *src) : src(src)
{
	id = 0;
}

void Shader::Use()
{
	if (this == active)
		return;

	glUseProgram(id);

	sceneDef->BindLights(this);

	SetMat4("view", cameraCur->view);
	SetMat4("projection", cameraCur->projection);

	active = this;
}

std::string Shader::CommonHeader()
{
	std::string header;
	if (!renderSettings.diffuseMaps)
		header += "#define DONT_USE_DIFFUSE_MAP\n";
	if (!renderSettings.normalMaps)
		header += "#define DONT_USE_NORMAL_MAP\n";
	if (!renderSettings.specularMaps)
		header += "#define DONT_USE_SPECULAR_MAP\n";
	if (!renderSettings.glowMaps)
		header += "#define DONT_USE_GLOW_MAP\n";
	if (!renderSettings.dust)
		header += "#define DONT_USE_DUST\n";
	return header;
}

void Shader::Compile()
{
	if (id)
		glDeleteProgram(id);

	std::string vert = header + CommonHeader() + ((*src)[1]);
	std::string frag = header + CommonHeader() + ((*src)[2]);

	GLuint vertex, fragment;

	const char *vert_lval = vert.c_str();
	const char *frag_lval = frag.c_str();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vert_lval, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_lval, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	//log_("made ur shader");
}

void Shader::SetInt(const char *name, int value) const { glUniform1i(glGetUniformLocation(id, name), value); }
void Shader::SetBool(const char *name, bool value) const { glUniform1i(glGetUniformLocation(id, name), (int)value); }
void Shader::SetFloat(const char *name, float value) const { glUniform1f(glGetUniformLocation(id, name), value); }
void Shader::SetVec2(const char *name, const vec2 &value) const { glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetVec3(const char *name, const vec3 &value) const { glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetVec4(const char *name, const vec4 &value) const { glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetMat2(const char *name, const mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }
void Shader::SetMat3(const char *name, const mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }
void Shader::SetMat4(const char *name, const mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			printf("glGetShaderInfoLog for %s\n %s\n", type.c_str(), infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("glGetProgramInfoLog for %s\n %s\n", type.c_str(), infoLog);
		}
	}
}
