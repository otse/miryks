#include <opengl/rt.h>

#include <opengl/shader.h>

#include <opengl/group.h>
#include <opengl/geometry.h>
#include <opengl/material.h>
#include <opengl/shader.h>

RenderTarget::RenderTarget(int width, int height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);  
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); 
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//// Alternative : Depth texture. Slower, but you can sample it later in your shader
	//GLuint depthTexture;
	//glGenTextures(1, &depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, 1024, 768, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//// Depth texture alternative :
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("bad fb");
}

void RenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderTarget::~RenderTarget()
{
}

Quadt::Quadt()
{
	shader = new Shader(&postquad);
	shader->header = "#version 330 core\n";
	shader->Compile();

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	static const GLfloat texas[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texas), texas, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);

	glBindVertexArray(0);
}

// https://github.com/opengl-tutorials/ogl/blob/master/tutorial14_render_to_texture/tutorial14.cpp
void Quadt::Draw(RenderTarget *rt)
{
	glDisable(GL_CULL_FACE);

	glUseProgram(shader->id);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rt->texture);
	glUniform1i(glGetUniformLocation(shader->id, "texture"), 0);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}