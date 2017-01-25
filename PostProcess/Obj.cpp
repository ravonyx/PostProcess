#include "Obj.h"
#include <fstream>
#include "Tools.h"
#include <iostream>
#include <ctime>

Obj::Obj(void) {}

Obj::~Obj(void){}

int findInVector(std::vector<Vertex>& mVec, Vertex key)
{
	int found = -1;
	int counter = 0;

	int size = mVec.size();
	Vertex *vertices = mVec.data();

	for (size_t i = 0; i < size; ++i)
	{
		if (vertices[i].v.x == key.v.x && vertices[i].v.y == key.v.y && vertices[i].v.z == key.v.z && 
			vertices[i].vt.x == key.vt.x && vertices[i].vt.y == key.vt.y && 
			vertices[i].vn.x == key.vn.x && vertices[i].vn.y == key.vn.y && vertices[i].vn.z == key.vn.z)
		{
			found = counter;
			break;
		}
		counter++;
	}

	return found;
}

bool Obj::load(const char* path)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices,  temp_normals;
	std::vector< glm::vec2 > temp_uvs;

	std::ifstream file;

	std::vector<Vertex> points;
	std::vector<int> indices;

	file.open(std::string(path), std::fstream::in);
	if (file.is_open() == false)
	{
		printf("Impossible to open the file !\n");
		return false;
	}
	std::cout << "--- Read Obj ---" << std::endl;
	int now = std::time(0);
	while (1)
	{
		if (file.good() == false)
			break;
		std::string lineaHeader;
		file >> lineaHeader;

		if (lineaHeader == "v")
		{
			glm::vec3 vertex;
			file >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (lineaHeader == "vt")
		{
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (lineaHeader == "vn")
		{
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (lineaHeader == "f")
		{
			unsigned int vertexIndex[3];
			unsigned int uvIndex[3];
			unsigned int normalIndex[3];
			unsigned int junk;

			if (temp_normals.empty() && temp_uvs.empty())
			{
				file >> vertexIndex[0]; file.ignore(1);
				file >> vertexIndex[1]; file.ignore(1);
				file >> vertexIndex[2];
			}
			else if(temp_uvs.empty())
			{
				file >> vertexIndex[0]; file.ignore(1); file.ignore(1); file >> normalIndex[0];
				file >> vertexIndex[1]; file.ignore(1); file.ignore(1); file >> normalIndex[1];
				file >> vertexIndex[2]; file.ignore(1); file.ignore(1); file >> normalIndex[2];
			}
			else if (temp_normals.empty())
			{
				file >> vertexIndex[0]; file.ignore(1); file >> uvIndex[0]; file.ignore(1);
				file >> vertexIndex[1]; file.ignore(1); file >> uvIndex[1]; file.ignore(1);
				file >> vertexIndex[2]; file.ignore(1); file >> uvIndex[2]; file.ignore(1);
			}
			else
			{
				file >> vertexIndex[0]; file.ignore(1); file >> uvIndex[0]; file.ignore(1); file >> normalIndex[0];
				file >> vertexIndex[1]; file.ignore(1); file >> uvIndex[1]; file.ignore(1); file >> normalIndex[1];
				file >> vertexIndex[2]; file.ignore(1); file >> uvIndex[2]; file.ignore(1); file >> normalIndex[2];
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);

			if (!temp_uvs.empty())
			{
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
			}
			if (!temp_normals.empty())
			{
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}

			for (int i = 0; i < 3; i++)
			{
				Vertex vertex;
				vertex.v = temp_vertices[vertexIndex[i] - 1];
				if (uvIndices.size() != 0)
					vertex.vt = temp_uvs[uvIndex[i] - 1];
				if (normalIndices.size() != 0)
					vertex.vn = temp_normals[normalIndex[i] - 1];
				int found = findInVector(points, vertex);
				if (found == -1)
				{
					points.push_back(vertex);
					indices.push_back(points.size() - 1);
				}
				else
					indices.push_back(found);
			}
		}
	}
	file.close();
	std::cout <<  time(0) - now << " seconds to load" << std::endl;
	std::cout << "--- Obj load ---" << std::endl;

	glGenVertexArrays(1, &vaoObj);
	glBindVertexArray(vaoObj);
	
	GLuint g_vbo;
	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * 8 * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(0);

	if (temp_uvs.size() > 0)
	{
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (temp_normals.size() > 0)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()* sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	size = indices.size();
	return true;
}

void Obj::render()
{
	glBindVertexArray(vaoObj);
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}
