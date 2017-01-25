#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include "glm\glm.hpp"

class Texture
{
	public:
		Texture();
		bool LoadImg(const std::string& name);
		bool LoadRepeatImg(const std::string& name);
		void LoadFromSample(std::vector<glm::vec3> &ssaoNoise);
		GLuint texId;
	private:
		std::string name;
		bool forceAlpha;
};

