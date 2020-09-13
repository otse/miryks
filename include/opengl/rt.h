#ifndef OPENGL_RT_H
#define OPENGL_RT_H

#include <opengl/types.h>

struct RT
{
    GLuint framebufferName, renderedTexture, depthrenderbuffer;
    void Bind();
	RT(int, int);
	~RT();
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