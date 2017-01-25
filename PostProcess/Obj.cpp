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

bool Obj::loadObj(const char* path, int programID, int programEnvMappingID)
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
	std::cout << "Open obj ..." << std::endl;

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
	std::cout << time(0) - now << std::endl;
	std::cout << "Obj load !" << std::endl;

	glGenVertexArrays(1, &vaoObj);
	glBindVertexArray(vaoObj);
	
	GLuint g_vbo;
	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * 8 * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(0);

	use_normal = true;
	use_tex = true;
	if (temp_uvs.size() > 0)
	{
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	else
		use_tex = false;
	if (temp_normals.size() > 0)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	else
		use_normal = false;

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()* sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	size = indices.size();
	texture_loc = glGetUniformLocation(programID, "texDiffuse");
	use_mat_loc = glGetUniformLocation(programID, "material");
	obj_only_loc = glGetUniformLocation(programID, "objOnly");
	use_normals_loc = glGetUniformLocation(programID, "useNormals");
	use_tex_diffuse_loc = glGetUniformLocation(programID, "useTexDiffuse");
	light_dir_loc = glGetUniformLocation(programID, "light_dir");
	light_mult_loc = glGetUniformLocation(programID, "light_multiplier");

	attr_cam_pos = glGetUniformLocation(programID, "camPos");
	attr_diffuse = glGetUniformLocation(programID, "diffuse");
	attr_ambient = glGetUniformLocation(programID, "ambient");
	attr_spec = glGetUniformLocation(programID, "specular");
	attr_shininess = glGetUniformLocation(programID, "shininess");
	flip_text_loc = glGetUniformLocation(programID, "flipText");

	attr_cam_env = glGetUniformLocation(programEnvMappingID, "camPos");
	skybox_location = glGetUniformLocation(programEnvMappingID, "cube_texture");

	return true;
}

void Obj::Draw(glm::vec3 camPos, glm::vec3 lightDirection, GLfloat lightMultiplier)
{

	
		if (strstr(mat.texDiffuse.c_str(), "dds") != NULL)
			glUniform1i(flip_text_loc, 1);
		else
			glUniform1i(flip_text_loc, 0);

		glUniform3fv(light_dir_loc, 1, &lightDirection[0]);
		glUniform1f(light_mult_loc, lightMultiplier);
		glUniform3fv(attr_cam_pos, 1, &camPos[0]);

		glUniform1i(obj_only_loc, use_obj_only);

		if (use_normal)
			glUniform1i(use_normals_loc, 1);
		else
			glUniform1i(use_normals_loc, 0);
		if (use_mat)
			glUniform1i(use_mat_loc, 1);
		else
			glUniform1i(use_mat_loc, 0);

		if (tex != -1)
		{
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(texture_loc, 0);
		}
		else
		{
			if (!mat.texDiffuse.empty())
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texDiffuse);
				glUniform1i(texture_loc, 0);
				glUniform1i(use_tex_diffuse_loc, 1);
			}
			else
				glUniform1i(use_tex_diffuse_loc, 0);

			glUniform3fv(attr_diffuse, 1, &mat.diffuse[0]);
			glUniform3fv(attr_ambient, 1, &mat.ambient[0]);
			glUniform3fv(attr_spec, 1, &mat.specular[0]);
			glUniform1f(attr_shininess, mat.shininess);
		}
	
	glBindVertexArray(vaoObj);
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}
