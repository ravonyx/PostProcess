#include <GL/glew.h>
#include <glm.hpp>
#include <gtx\transform.hpp>

#include <iostream>
#include <AntTweakBar.h>
#include <stdio.h>
#include "glut.h"

#include "EsgiShader.h"
#include "Camera.h"
#include "Obj.h"

int renderProgram;

Camera *cam;
Obj	model;

int post_effect, env_mapping;
float ratio, angle;
float scale;
Quaternion rotation;
glm::vec3 light_direction;

bool mode_ui = true;

void reshape(int w, int h);
void initScene();
void render(void);

void TW_CALL SetPostEffectCB(const void *value, void *clientData);
void TW_CALL GetPostEffectCB(void *value, void *clientData);
void TW_CALL SetEnvMappingCB(const void *value, void *clientData);
void TW_CALL GetEnvMappingCB(void *value, void *clientData);

void Initialize();
void Terminate();

struct
{
	struct
	{
		GLint           model_matrix;
		GLint           viewproj_matrix;
		GLint           light_direction;
		GLint           cam_position;
		GLint           shading_level;
	} render;

	struct
	{
		GLint           ssao_level;
		GLint           object_level;
		GLint           ssao_radius;
		GLint           weight_by_angle;
		GLint           randomize_points;
		GLint           point_count;
	} ssao;
} uniforms;

int main(int argc, char **argv)
{

	/** CREATION FENETRE **/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Loader Obj");
	Initialize();

	/** FONCTIONS GLUT **/
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	TwInit(TW_OPENGL, NULL);

	glutIdleFunc(render);
	initScene();

	/** GESTION SOURIS **/
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

	// Create a tweak bar
	TwBar *bar;
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='250 500' color='96 216 224' "); 
	TwAddVarCB(bar, "Post effect", TW_TYPE_BOOL32, SetPostEffectCB, GetPostEffectCB, NULL, " label='Post effect' key=space help='Toggle post effect mode.' ");
	TwAddVarCB(bar, "Env Mapping effect", TW_TYPE_BOOL32, SetEnvMappingCB, GetEnvMappingCB, NULL, " label='Env Mapping' key=e help='Toggle env mapping mode.' ");
	TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &scale, " min=0.1 max=20 step=0.1 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation, " label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &light_direction, " label='Light direction' opened=true help='Change the light direction.' ");

	glutMainLoop();
	Terminate();
	return (1);
}

void Initialize()
{
	printf("Version OpenGL : %s\n", glGetString(GL_VERSION));
	printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	printf("Pilote : %s\n", glGetString(GL_RENDERER));
	printf("Version GLSL : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glewInit();
}
void Terminate()
{
	TwTerminate();
}

/** GESTION FENETRE **/
void reshape(int w, int h)
{
	if (h == 0)
		h = 1;
	ratio = w * 1.0 / h;
	glViewport(0, 0, w, h);
	TwWindowSize(w, h);
}

/** AFFICHAGE **/
void initScene()
{
	EsgiShader renderShader;

	printf("Load Fragment obj shader\n");
	renderShader.LoadFragmentShader("render.fs");
	printf("Load Vertex obj shader\n");
	renderShader.LoadVertexShader("render.vs");
	renderShader.Create();

	renderProgram = renderShader.GetProgram();
	glUseProgram(renderProgram);
	uniforms.render.model_matrix = glGetUniformLocation(renderProgram, "model_matrix");
	uniforms.render.viewproj_matrix = glGetUniformLocation(renderProgram, "viewproj_matrix");
	uniforms.render.light_direction = glGetUniformLocation(renderProgram, "light_direction");
	uniforms.render.cam_position = glGetUniformLocation(renderProgram, "cam_position");
	uniforms.render.shading_level = glGetUniformLocation(renderProgram, "shading_level");

	cam = new Camera();
	light_direction = glm::vec3(-0.5f, -0.5f, -0.5f);
	rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	scale = 1.0f;

	post_effect = 0;
	env_mapping = 0;

	model.load("objects/charizard.obj");
}
void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// D�finition de la cam�ra
	glm::vec3 camPos = glm::vec3(-(cam->posx), -(cam->posy), -(cam->posz));
	glm::mat4 proj = glm::perspective(45.0f, ratio, 0.1f, 400.0f);
	glm::mat4 view = cam->GetOrientation() * glm::translate(camPos);
	glm::mat4 proj_view = proj * view;
	glm::mat4 model_mat = glm::translate(glm::vec3(0, 2, 0)) * rotation.QuaternionToMatrix() *  glm::scale(glm::vec3(scale, -scale, scale));

	glUseProgram(renderProgram);
	glUniformMatrix4fv(uniforms.render.viewproj_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glUniformMatrix4fv(uniforms.render.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	glm::vec3 light_dir = -light_direction;
	glUniform3fv(uniforms.render.light_direction, 1, &light_dir[0]);
	glUniform3fv(uniforms.render.cam_position, 1, &camPos[0]);
	model.render();

	TwDraw();
	glutSwapBuffers();
}

void TW_CALL SetPostEffectCB(const void *value, void *clientData)
{
	(void)clientData; 
	post_effect = *(const int *)value;
}
void TW_CALL GetPostEffectCB(void *value, void *clientData)
{
	(void)clientData; 
	*(int *)value = post_effect; 
}

void TW_CALL SetEnvMappingCB(const void *value, void *clientData)
{
	(void)clientData;
	env_mapping = *(const int *)value;
}
void TW_CALL GetEnvMappingCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = env_mapping;
}