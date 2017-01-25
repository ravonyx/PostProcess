#pragma once

#include <GL/glew.h>

class Framebuffer
{
	public:
		Framebuffer(GLuint width, GLuint height);
		void Init();
		~Framebuffer();
		GLuint vao, vbo;
		GLuint gBuffer, gDepth, colorTexture, normalTexture, specularTexture, positionTexture;
};

