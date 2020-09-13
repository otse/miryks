#ifndef OPENGL_RT_H
#define OPENGL_RT_H

#include "types.h"

struct RT
{
    GLuint fb, tid, depth;

	RT();
	~RT();

    void Bind();

};

struct RTQuad
{
    RTQuad();
};

#endif