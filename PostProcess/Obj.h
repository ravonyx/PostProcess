#ifndef _OBJ_H
#define _OBJ_H

#include <glm.hpp>
#include <GL/glew.h>
#include <string>
#include <vector>

class Obj
{
	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse;
		float shininess;
		std::string texAmbient;
		std::string texDiffuse;
		std::string texSpecular;
	};

	public:
		Obj(void);
		virtual ~Obj(void);
		bool	loadObj(const char* path, int programID, int programEnvMappingID);
		void    Draw(glm::vec3 camPos, glm::vec3 lightDirection, GLfloat lightMultiplier);

	private:
		bool use_normal, use_tex, use_obj_only, use_mat;
		GLuint tex;
		Material mat;
		int _textureId;

		int size;

		GLuint texAmbient;
		GLuint texDiffuse;
		GLuint texSpecular;

		GLuint vaoObj;

		GLuint use_mat_loc, use_tex_diffuse_loc, use_env_mapping, attr_cam_env, use_normals_loc, light_dir_loc, light_mult_loc, texture_loc, obj_only_loc;
		GLuint attr_cam_pos, attr_diffuse, attr_ambient, attr_spec, attr_shininess, skybox_location, flip_text_loc;
};

#endif