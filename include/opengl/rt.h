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

struct RTQuad
{
    Geometry *geometry;
    Shader *shader;
    GLuint quad_VertexArrayID, quad_vertexbuffer;
    GLuint texID, timeID;
    void Draw();
    RTQuad();
};

#endif