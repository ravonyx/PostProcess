#include "Texture.h"

#include "stb_image.h"

Texture::Texture()
{
	texId = -1;
}

bool Texture::LoadImg(const std::string& name)
{
	int w, h, depth;
	auto* data = stbi_load(name.c_str(), &w, &h, &depth, true ? STBI_rgb_alpha : STBI_rgb);
	if (data)
	{
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
			, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		return true;
	}
	texId = -1;
	return false;
}

bool Texture::LoadRepeatImg(const std::string& name)
{
	int w, h, depth;
	auto* data = stbi_load(name.c_str(), &w, &h, &depth
		, true ? STBI_rgb_alpha : STBI_rgb);
	if (data)
	{
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
			, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		return true;
	}
	texId = -1;
	return false;
}

void Texture::LoadFromSample(std::vector<glm::vec3> &ssaoNoise)
{
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}