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
#include "Framebuffer.h"
#include "Tools.h"

TwBar *bar;
int width = 1200;
int height = 800;

Framebuffer *fbo;
int renderProgram, blitProgram, blitDepthProgram, ssaoProgram;

Camera *cam;
Obj	object, cube;

int ssao, dof;
float ratio, angle, scale;
Quaternion rotation;
glm::vec3 light_direction;

bool mode_ui = true;

void reshape(int w, int h);
void initScene();
void loadShaders();
void render(void);
void displayDebug();

void TW_CALL SetSSAOCB(const void *value, void *clientData);
void TW_CALL GetSSAOCB(void *value, void *clientData);
void TW_CALL SetDOFCB(const void *value, void *clientData);
void TW_CALL GetDOFCB(void *value, void *clientData);
static  void __stdcall exitCallbackTw(void* clientData);

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
		bool  show_shading;
		bool  show_ao;
		float level;
		float radius;
		bool  weight_by_angle;
		bool randomize_points;
		unsigned int point_count;
	} ssao;
} vars;

GLuint points_buffer;
struct SAMPLE_POINTS
{
	glm::vec4     point[256];
	glm::vec4     random_vectors[256];
};

int main(int argc, char **argv)
{

	/** CREATION FENETRE **/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Post processing");
	glewInit();

	/** FONCTIONS GLUT **/
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	TwInit(TW_OPENGL, NULL);

	loadShaders();
	initScene();

	/** GESTION SOURIS **/
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

	// Create a tweak bar
	bar = TwNewBar("TweakBar");
	TwWindowSize(width, height);
	TwDefine(" TweakBar size='250 500' color='200 200 200' "); 
	TwAddVarCB(bar, "SSAO", TW_TYPE_BOOL32, SetSSAOCB, GetSSAOCB, NULL, " label='SSAO' key=e help='Screen space ambient occlusion' ");
	TwAddVarCB(bar, "DOF", TW_TYPE_BOOL32, SetDOFCB, GetDOFCB, NULL, " label='DOF' key=e help='Depth of field' ");

	TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &scale, " min=0.1 max=20 step=0.1 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation, " label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &light_direction, " label='Light direction' opened=true help='Change the light direction.' ");
	TwAddSeparator(bar, "program", "");
	TwAddButton(bar, "Exit", &exitCallbackTw, nullptr, "");

	glutMainLoop();
	return 1;
}

/** GESTION FENETRE **/
void reshape(int w, int h)
{
	width = w;
	height = h;

	ratio = width * 1.0 / height;
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

/** INIT **/
void loadShaders()
{
	EsgiShader renderShader, blitShader, blitDepthShader, ssaoShader;

	printf("render fs\n");
	renderShader.LoadFragmentShader("shaders/render.fs");
	printf("render vs\n");
	renderShader.LoadVertexShader("shaders/render.vs");
	renderShader.Create();

	printf("blit fs\n");
	blitShader.LoadFragmentShader("shaders/blit.fs");
	printf("blit vs\n");
	blitShader.LoadVertexShader("shaders/blit.vs");
	blitShader.Create();

	printf("blitDepth fs\n");
	blitDepthShader.LoadFragmentShader("shaders/blitDepth.fs");
	printf("blit vs\n");
	blitDepthShader.LoadVertexShader("shaders/blit.vs");
	blitDepthShader.Create();

	printf("ssao fs\n");
	ssaoShader.LoadFragmentShader("shaders/ssao.fs");
	printf("ssao vs\n");
	ssaoShader.LoadVertexShader("shaders/ssao.vs");
	ssaoShader.Create();

	renderProgram = renderShader.GetProgram();
	blitProgram = blitShader.GetProgram();
	blitDepthProgram = blitDepthShader.GetProgram();
	ssaoProgram = ssaoShader.GetProgram();

	glUseProgram(renderProgram);
	uniforms.render.model_matrix = glGetUniformLocation(renderProgram, "model_matrix");
	uniforms.render.viewproj_matrix = glGetUniformLocation(renderProgram, "viewproj_matrix");
	uniforms.render.light_direction = glGetUniformLocation(renderProgram, "light_direction");
	uniforms.render.cam_position = glGetUniformLocation(renderProgram, "cam_position");
	uniforms.render.shading_level = glGetUniformLocation(renderProgram, "shading_level");

	uniforms.ssao.ssao_radius = glGetUniformLocation(ssaoProgram, "ssao_radius");
	uniforms.ssao.ssao_level = glGetUniformLocation(ssaoProgram, "ssao_level");
	uniforms.ssao.object_level = glGetUniformLocation(ssaoProgram, "object_level");
	uniforms.ssao.weight_by_angle = glGetUniformLocation(ssaoProgram, "weight_by_angle");
	uniforms.ssao.randomize_points = glGetUniformLocation(ssaoProgram, "randomize_points");
	uniforms.ssao.point_count = glGetUniformLocation(ssaoProgram, "point_count"); 
}

void initScene()
{
	cam = new Camera(0.0f, -2.0f, 0.0f, -1.0f);
	light_direction = glm::vec3(-0.5f, -0.5f, -0.5f);

	rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	float axis[3] = { 0, 0, 1 };
	rotation.QuaternionFromAxis(axis, 180);
	scale = 1.0f;

	ssao = 0;
	dof = 0;

	fbo = new Framebuffer(width, height);
	fbo->Init();

	object.load("objects/charizard.obj");
	cube.load("objects/box.obj");

	//SSAO init
	SAMPLE_POINTS point_data;
	for (int i = 0; i < 256; i++)
	{
		do
		{
			point_data.point[i][0] = random_float() * 2.0f - 1.0f;
			point_data.point[i][1] = random_float() * 2.0f - 1.0f;
			point_data.point[i][2] = random_float(); //  * 2.0f - 1.0f;
			point_data.point[i][3] = 0.0f;
		} while (length(point_data.point[i]) > 1.0f);
		normalize(point_data.point[i]);
	}
	for (int i = 0; i < 256; i++)
	{
		point_data.random_vectors[i][0] = random_float();
		point_data.random_vectors[i][1] = random_float();
		point_data.random_vectors[i][2] = random_float();
		point_data.random_vectors[i][3] = random_float();
	}

	glGenBuffers(1, &points_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);


	vars.ssao.level = 1.0f;
	vars.ssao.radius = 0.05f;
	vars.ssao.show_shading = true;
	vars.ssao.show_ao = true;
	vars.ssao.weight_by_angle = true;
	vars.ssao.randomize_points = true;
	vars.ssao.point_count = 10;
}

void render(void)
{
	glViewport(0, 0, width, height);

	//render in framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BACK);

	// Définition de la caméra
	glm::vec3 camPos = glm::vec3(cam->posx, cam->posy, cam->posz);
	glm::mat4 proj = glm::perspective(45.0f, ratio, 0.1f, 400.0f);
	glm::mat4 view = cam->GetOrientation() * glm::translate(camPos);
	glm::mat4 proj_view = proj * view;
	glm::mat4 model_mat;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);

	glUseProgram(renderProgram);
	glUniformMatrix4fv(uniforms.render.viewproj_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glm::vec3 light_dir = -light_direction;
	glUniform3fv(uniforms.render.light_direction, 1, &light_dir[0]);
	glUniform3fv(uniforms.render.cam_position, 1, &camPos[0]);
	

	model_mat = glm::translate(glm::vec3(0, -12, -15))  *  glm::scale(glm::vec3(10, 10, 10));
	glUniformMatrix4fv(uniforms.render.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	cube.render();
	model_mat = glm::translate(glm::vec3(0, 2, -15)) * rotation.QuaternionToMatrix() *  glm::scale(glm::vec3(1, 1, 1));
	glUniformMatrix4fv(uniforms.render.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	object.render();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ssao)
	{
		glUseProgram(ssaoProgram);

		glUniform1f(uniforms.ssao.ssao_radius, vars.ssao.radius * float(width) / 1000.0f);
		glUniform1f(uniforms.ssao.ssao_level, vars.ssao.show_ao ? (vars.ssao.show_shading ? 0.3f : 1.0f) : 0.0f);
		// glUniform1i(uniforms.ssao.weight_by_angle, weight_by_angle ? 1 : 0);
		glUniform1i(uniforms.ssao.randomize_points, vars.ssao.randomize_points ? 1 : 0);
		glUniform1ui(uniforms.ssao.point_count, vars.ssao.point_count);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo->colorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbo->normalTexture);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(fbo->vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	else if (dof)
	{

	}
	else
	{
		//render in screen
		model_mat = glm::translate(glm::vec3(0, -12, -15))  *  glm::scale(glm::vec3(10, 10, 10));
		glUniformMatrix4fv(uniforms.render.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
		cube.render();
		model_mat = glm::translate(glm::vec3(0, 2, -15)) * rotation.QuaternionToMatrix() *  glm::scale(glm::vec3(1, 1, 1));
		glUniformMatrix4fv(uniforms.render.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
		object.render();
	}
	//Render 4 quads at bottom
	displayDebug();
	
	TwDraw();
	glutSwapBuffers();
	glutPostRedisplay();
}

void displayDebug()
{
	GLuint textLoc = glGetUniformLocation(blitProgram, "texture_data");
	GLuint textDepthLoc = glGetUniformLocation(blitDepthProgram, "texture_data");

	glUseProgram(blitProgram);
	glActiveTexture(GL_TEXTURE0);

	glViewport(0, 0, width / 4, height / 5);
	glBindVertexArray(fbo->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo->colorTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glViewport(width / 4, 0, width / 4, height / 5);
	glBindVertexArray(fbo->vao);
	glBindTexture(GL_TEXTURE_2D, fbo->normalTexture);
	glUniform1i(textLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glViewport(width / 4 + width / 4, 0, width / 4, height / 5);
	glBindVertexArray(fbo->vao);
	glBindTexture(GL_TEXTURE_2D, fbo->specularTexture);
	glUniform1i(textLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glUseProgram(blitDepthProgram);
	glViewport(width / 4 + width / 4 + width / 4, 0, width / 4, height / 5);
	glBindVertexArray(fbo->vao);
	glBindTexture(GL_TEXTURE_2D, fbo->gDepth);
	glUniform1i(textDepthLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void TW_CALL SetSSAOCB(const void *value, void *clientData)
{
	(void)clientData; 
	ssao = *(const int *)value;
}
void TW_CALL GetSSAOCB(void *value, void *clientData)
{
	(void)clientData; 
	*(int *)value = ssao;
}

void TW_CALL SetDOFCB(const void *value, void *clientData)
{
	(void)clientData;
	dof = *(const int *)value;
}
void TW_CALL GetDOFCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = dof;
}

static  void __stdcall exitCallbackTw(void* clientData)
{
	TwTerminate();
	exit(1);
}