#include "Ssao.h"
#include <random>

GLfloat Ssao::lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}

Ssao::Ssao()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	std::default_random_engine generator;

	for (GLuint i = 0; i < kernelSize; ++i)
	{
		glm::vec3 sample
		(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		GLfloat scale = 1;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		kernel.push_back(sample);
	}
	std::vector<glm::vec3> noiseKernel;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		noiseKernel.push_back(noise);
	}

	noiseTexture = new Texture();
	noiseTexture->LoadFromSample(noiseKernel);
}
