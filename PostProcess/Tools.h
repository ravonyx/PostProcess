#include <GL/glew.h>
#include <string>
#include <glm.hpp>
#include <vector>

// FBO
struct Framebuffer
{
	GLuint colorBuffer;
	GLuint depthBuffer;
	GLuint FBO;
	GLuint vao;

	bool CreateFBO(GLuint w, GLuint h);
	void DestroyFBO();
	void Init(int programID);
};

struct Vertex
{
	glm::vec3 v;
	glm::vec2 vt;
	glm::vec3 vn;
	
};


GLuint CreateTexture(const char* name);
GLuint CreateEmptyTexture(GLuint w, GLuint h);
GLuint CreateDepthTexture(GLuint w, GLuint h);
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);

void ListFolders(std::vector<std::string> &objNames, const std::string &path);

bool skipSpace(std::string &line, size_t &offset);
bool getWord(std::string &line, size_t &offset, std::string &outWord);
float getFloat(std::string &line, size_t &offset);