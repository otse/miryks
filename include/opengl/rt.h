#ifndef OPENGL_RT_H
#define OPENGL_RT_H

#include <opengl/types.h>

struct RenderTarget
{
    GLuint framebufferName, renderedTexture, depthrenderbuffer;
    void Bind();
	RenderTarget(int, int);
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