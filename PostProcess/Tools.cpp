#include "Tools.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Windows.h>

void ListFolders(std::vector<std::string> &objNames, const std::string &path)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	char sPath[2048];
	sprintf(sPath, "%s\\*.*", path.c_str());

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", path.c_str());
		return;
	}

	do
	{
		if (strcmp(fdFile.cFileName, ".") != 0
			&& strcmp(fdFile.cFileName, "..") != 0)
		{
			if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
				objNames.push_back(fdFile.cFileName);
		}
	} 
	while (FindNextFile(hFind, &fdFile)); //Find the next file.
	FindClose(hFind); //Always, Always, clean things up!
}

GLuint CreateTexture(const char* name)
{
	int w, h, comp;
	int req_comp = 4;

	auto* image = stbi_load(name, &w, &h, &comp, req_comp);
	if (!image)
	{
		fprintf(stderr, "ERROR: could not load %s\n", name);
		return -1;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	return texture;
}

bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name) 
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;
	int force_channels = 4;
	unsigned char*  image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) 
	{
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		getchar();
		return false;
	}

	// non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0)
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);

	// copy image data into 'target' side of cube map
	glTexImage2D(side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	free(image_data);
	return true;
}


GLuint CreateEmptyTexture(GLuint w, GLuint h)
{
	GLuint textureObj;
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return textureObj;
}

GLuint CreateDepthTexture(GLuint w, GLuint h)
{
	GLuint textureObj;
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	return textureObj;
}


bool Framebuffer::CreateFBO(GLuint w, GLuint h)
{
	colorBuffer = CreateEmptyTexture(w, h);
	depthBuffer = CreateDepthTexture(w, h);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return (check == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::Init(int programID)
{
	float vertices[] = {
		1.0f,  1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};

	//Framebuffer init
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint g_vbo;
	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(programID, "a_position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);
	GLint coordAttrib = glGetAttribLocation(programID, "a_textCoords");
	glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(coordAttrib);

	glBindVertexArray(0);
}

void Framebuffer::DestroyFBO()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &colorBuffer);
	glDeleteTextures(1, &depthBuffer);
}

bool skipSpace(std::string &line, size_t &offset)
{
	while (line[offset] == ' ' || line[offset] == '\t')
	{
		offset++;
		if (offset >= line.size())
			return false;
	}
	return true;
}

bool getWord(std::string &line, size_t &offset, std::string &outWord)
{
	size_t tmpOffset = offset;
	while (line[tmpOffset] != ' ' && line[tmpOffset] != '\t' && tmpOffset < line.size())
	{
		tmpOffset++;
	}
	const size_t count = tmpOffset - offset;
	if (count == 0)
		return false;
	outWord = line.substr(offset, count);
	offset = tmpOffset;
	if (offset >= line.size())
		return false;
	return true;
}

float getFloat(std::string &line, size_t &offset)
{
	size_t tmpOffset = offset;
	while (line[tmpOffset] != ' ' && line[tmpOffset] != '\t' && tmpOffset < line.size())
	{
		tmpOffset++;
	}
	const size_t count = tmpOffset - offset;
	if (count == 0)
		return 0.0f;
	if (offset >= line.size())
		return 0.0f;
	std::string substr = line.substr(offset, count);
	offset = tmpOffset;
	return stof(substr);
}
