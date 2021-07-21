#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

// part of gloom

#include <opengl/Types.h>

// https://learnopengl.com/Getting-started/Textures

Texture *GetProduceTexture(const char *);

struct Texture
{
	friend Texture *GetProduceTexture(const char *);
	GLuint tid;
	int size;
	const unsigned char *buf = nullptr;
	const std::string path;
//protected:
	Texture();
	Texture(const std::string &);
	~Texture();
	void load();
};

#endif