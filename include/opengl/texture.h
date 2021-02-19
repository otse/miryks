#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include <OpenGL/Types.h>

// https://learnopengl.com/Getting-started/Textures

struct Texture
{
    friend Texture *GetProduceTexture(const char *);

    GLuint tid;

    int size;
    const unsigned char *buf = nullptr;

    const std::string path;

protected:
    Texture(const std::string &);
    ~Texture();

    void Load();
};

#endif