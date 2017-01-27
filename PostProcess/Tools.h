#include <GL/glew.h>
#include <string>
#include <glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 v;
	glm::vec2 vt;
	glm::vec3 vn;
	
};

bool skipSpace(std::string &line, size_t &offset);
bool getWord(std::string &line, size_t &offset, std::string &outWord);
float getFloat(std::string &line, size_t &offset);

static unsigned int seed = 0x13371337;
static float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}