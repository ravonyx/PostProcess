#ifndef _OBJ_H
#define _OBJ_H

#include <glm.hpp>
#include <GL/glew.h>
#include <string>
#include <vector>

class Obj
{
	public:
		Obj(void);
		virtual ~Obj(void);
		bool	load(const char* path);
		void    render();

	private:
		int size;
		GLuint vaoObj;
};

#endif