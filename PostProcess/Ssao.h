#pragma once
#include "Texture.h"

#include <vector>
#include "glm\glm.hpp"

class Ssao
{
	private:
		std::vector<glm::vec3> kernel;
		int kernelSize = 64;
		GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

	public:
		Texture *noiseTexture;
		Ssao();
		int getKernelSize()
		{
			return kernelSize;
		}
		std::vector<glm::vec3>& getKernel()
		{
			return kernel;
		}
};