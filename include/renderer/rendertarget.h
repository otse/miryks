#ifndef OPENGL_RT_H
#define OPENGL_RT_H

// part of gloom

#include <renderer/renderer.h>

struct RenderTarget
{
    GLuint fbo, texture, rbo;
    void Bind();
	RenderTarget(int, int, GLenum, GLenum);
	~RenderTarget();
};

struct Quadt
{
    Quadt();
    //Geometry *geometry;
    Shader *shader;
    GLuint vao, vbo;
    //GLuint texID, timeID;
    void Draw(RenderTarget *);
};

#endif