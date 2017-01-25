#define FBXSDK_SHARED

#include "Quaternion.hpp"
#include "../common/common.h"
#include <iostream>
#include <AntTweakBar.h>
#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Ssao.h"
#include <glm\gtx\transform.hpp>
#include "Object.h"

Camera *cam;
glm::mat4 proj, view;
GLint width = 1500, height = 800;

Framebuffer *g_fbofirstPass, *g_fboSecondPass;

int renderProgram, postProgram, blitProgram, blitDepthProgram, ssaoProgram, aliasingProgram, dofProgram;

Ssao ssaoParameters;

//variables for ui;
Quaternion rotation;
bool post_effect, ssao, dof, aliasing;
glm::vec3 light_direction;
bool mode_ui;

Object object;
Object cube;

struct
{
	struct
	{
		GLint           mv_matrix;
		GLint           proj_matrix;
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

#pragma region headers
void Initialize();
void InitParameters();
void LoadShaders();
void LoadUI();

void Terminate();
void Resize(GLint width, GLint height);
void Render();
void ComputePos(int unused);
void KeyboardDown(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void MouseButton(int button, int state, int x, int y);
void MouseMove(int x, int y);

void TW_CALL SetPostEffectCB(const void *value, void *clientData);
void TW_CALL GetPostEffectCB(void *value, void *clientData);
void TW_CALL GetSSAOCB(void *value, void *clientData);
void TW_CALL SetSSAOCB(const void *value, void *clientData);
void TW_CALL SetAliasingCB(const void *value, void *clientData);
void TW_CALL GetAliasingCB(void *value, void *clientData);
void TW_CALL SetDOFCB(const void *value, void *clientData);
void TW_CALL GetDOFCB(void *value, void *clientData);
static  void __stdcall exitCallbackTw(void* clientData);

#pragma endregion

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("FBX Scene");

	TwInit(TW_OPENGL, NULL);

	Initialize();
	InitParameters();
	LoadShaders();
	LoadUI();

	glutMainLoop();
	Terminate();
	return 1;
}

void Resize(GLint width, GLint height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(45.0f, (float)width / height, 0.1f, 1000.0f);
	width = height;
	height = height;
	TwWindowSize(width, height);
}

void Render()
{
	glViewport(0, 0, width, height);
	//glBindFramebuffer(GL_FRAMEBUFFER, g_fbofirstPass->gBuffer);

	glClearColor(0.1f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//Draw in FBO
	glUseProgram(renderProgram);
	glm::vec3 camPos = glm::vec3(cam->posx, cam->posy, cam->posz);
	view = cam->GetOrientation() * glm::translate(camPos);

	glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, &proj[0][0]);


	float axis[3] = { 1, 0, 0 };
	rotation.QuaternionFromAxis(axis, 45);
	glm::mat4 op_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, -30.0f)) * rotation.QuaternionToMatrix() * glm::scale(glm::vec3(5.0, 5.0, 5.0));


	glm::mat4 mv_matrix = view * op_matrix;
	glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, &mv_matrix[0][0]);

	glUniform1f(uniforms.render.shading_level, 0.0f);
	object.render();

	//Post Effect
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(postProgram);
	glUniformMatrix4fv(glGetUniformLocation(postProgram, "u_projectionMatrix"), 1, GL_FALSE, &proj[0][0]);

	if (ssao == 1)
	{

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	else if (aliasing == 1)
	{
		glUniform1i(glGetUniformLocation(aliasingProgram, "width"), width);
		glUniform1i(glGetUniformLocation(aliasingProgram, "height"), height);

		glUseProgram(aliasingProgram);
		glViewport(0, 0, width, height);
		glBindVertexArray(g_fbofirstPass->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_fbofirstPass->colorTexture);
		glUniform1i(glGetUniformLocation(aliasingProgram, "textureColor"), 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	else
	{
		//Redraw to display in full screen
		model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -8.0f, 0.0f)) * rotation.QuaternionToMatrix();
	}
	

	//Display 4 FBO at bottom
	GLuint textLoc = glGetUniformLocation(blitProgram, "texture_data");
	GLuint textDepthLoc = glGetUniformLocation(blitDepthProgram, "texture_data");

	glUseProgram(blitProgram);
	glActiveTexture(GL_TEXTURE0);

	glViewport(0, 0, width / 4, height / 4);
	glBindVertexArray(g_fbofirstPass->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_fbofirstPass->colorTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glViewport(width / 4, 0, width / 4, height / 4);
	glBindVertexArray(g_fbofirstPass->vao);
	glBindTexture(GL_TEXTURE_2D, g_fbofirstPass->normalTexture);
	glUniform1i(textLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glViewport(width / 4 + width / 4, 0, width / 4, height / 4);
	glBindVertexArray(g_fbofirstPass->vao);
	glBindTexture(GL_TEXTURE_2D, g_fbofirstPass->specularTexture);
	glUniform1i(textLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glUseProgram(blitDepthProgram);

	glViewport(width / 4 + width / 4 + width /4, 0, width/4, height / 4);
	glBindVertexArray(g_fbofirstPass->vao);
	glBindTexture(GL_TEXTURE_2D, g_fbofirstPass->gDepth);
	glUniform1i(textDepthLoc, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	//GLenum errorGL = glGetError();
	//assert(errorGL == GL_NO_ERROR);*/
	
	TwDraw();
	glutSwapBuffers();
}

void Initialize()
{
	printf("Version Pilote OpenGL : %s\n", glGetString(GL_VERSION));
	printf("Type de GPU : %s\n", glGetString(GL_RENDERER));
	printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	printf("Version GLSL : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	//callback 
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(NULL);
	glutSpecialFunc(NULL);

	glutReshapeFunc(Resize);
	glutDisplayFunc(Render);
	glutTimerFunc(10, ComputePos, 0);
}

void InitParameters()
{
	//Camera
	cam = new Camera(0, 0, -50, -1);
	proj = glm::perspectiveFov(45.f, (float)width, (float)height, 0.1f, 1000.f);
	ssaoParameters = Ssao();

	//Framebuffer
	post_effect = 0;
	g_fbofirstPass = new Framebuffer(width, height);
	g_fbofirstPass->Init();
	g_fboSecondPass = new Framebuffer(width, height);
	g_fboSecondPass->Init();

	//Init variables
	mode_ui = false;
	rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	float axisX[3] = { 1.0f, 0.0f, 0.0f };
	rotation = rotation.QuaternionFromAxis(axisX, -90);
	light_direction = glm::vec3(-0.5f, -0.5f, -0.5f);

	object.load("sbm/dragon.sbm");
	cube.load("sbm/cube.sbm");
}

void LoadShaders()
{
	//Load Shaders
	EsgiShader g_renderShader, g_postShader, g_blitShader, g_blitDepthShader, g_ssaoShader, g_aliasingShader, g_dofShader;
	printf("Load main vs\n");
	g_renderShader.LoadVertexShader("shaders/main.vs");
	printf("Load main fs\n");
	g_renderShader.LoadFragmentShader("shaders/main.fs");
	g_renderShader.Create();

	printf("Load postProcess fs\n");
	g_postShader.LoadFragmentShader("shaders/postProcess.fs");
	printf("Load postProcess vs\n");
	g_postShader.LoadVertexShader("shaders/postProcess.vs");
	g_postShader.Create();

	g_blitShader.LoadFragmentShader("shaders/blit.fs");
	g_blitShader.LoadVertexShader("shaders/blit.vs");
	g_blitShader.Create();

	g_blitDepthShader.LoadFragmentShader("shaders/blitDepth.fs");
	g_blitDepthShader.LoadVertexShader("shaders/blit.vs");
	g_blitDepthShader.Create();

	printf("Load ssao fs\n");
	g_ssaoShader.LoadFragmentShader("shaders/ssao.fs");
	printf("Load ssao vs\n");
	g_ssaoShader.LoadVertexShader("shaders/postProcess.vs");
	g_ssaoShader.Create();

	g_aliasingShader.LoadFragmentShader("shaders/aliasing.fs");
	g_aliasingShader.LoadVertexShader("shaders/postProcess.vs");
	g_aliasingShader.Create();

	printf("Load dof fs\n");
	g_dofShader.LoadFragmentShader("shaders/dof.fs");
	printf("Load dof vs\n");
	g_dofShader.LoadVertexShader("shaders/postProcess.vs");
	g_dofShader.Create();

	renderProgram = g_renderShader.GetProgram();
	postProgram = g_postShader.GetProgram();
	blitProgram = g_blitShader.GetProgram();
	blitDepthProgram = g_blitDepthShader.GetProgram();
	ssaoProgram = g_ssaoShader.GetProgram();
	aliasingProgram = g_aliasingShader.GetProgram();

	uniforms.render.mv_matrix = glGetUniformLocation(renderProgram, "mv_matrix");
	uniforms.render.proj_matrix = glGetUniformLocation(renderProgram, "proj_matrix");
	uniforms.render.shading_level = glGetUniformLocation(renderProgram, "shading_level");

	uniforms.ssao.ssao_radius = glGetUniformLocation(ssaoProgram, "ssao_radius");
	uniforms.ssao.ssao_level = glGetUniformLocation(ssaoProgram, "ssao_level");
	uniforms.ssao.object_level = glGetUniformLocation(ssaoProgram, "object_level");
	uniforms.ssao.weight_by_angle = glGetUniformLocation(ssaoProgram, "weight_by_angle");
	uniforms.ssao.randomize_points = glGetUniformLocation(ssaoProgram, "randomize_points");
	uniforms.ssao.point_count = glGetUniformLocation(ssaoProgram, "point_count");

	GLenum errorGL = glGetError();
	assert(errorGL == GL_NO_ERROR);
}

void LoadUI()
{
	TwBar *bar;

	// Create a tweak bar
	bar = TwNewBar("PostProcessingAdvanced");

	TwDefine(" PostProcessingAdvanced size='250 500' color='86 101 115  ' ");
	TwAddVarCB(bar, "SSAO", TW_TYPE_BOOL32, SetSSAOCB, GetSSAOCB, NULL, " label='SSAO' key=4 help='Toggle ssao mode.' ");
	TwAddVarCB(bar, "Aliasing", TW_TYPE_BOOL32, SetAliasingCB, GetAliasingCB, NULL, " label='Aliasing' key=3 help='Toggle aliasing mode.' ");
	TwAddVarCB(bar, "DOF", TW_TYPE_BOOL32, SetDOFCB, GetDOFCB, NULL, " label='DOF' key=2 help='Toggle dof mode.' ");
	TwAddVarCB(bar, "PostProcess", TW_TYPE_BOOL32, SetPostEffectCB, GetPostEffectCB, NULL, " label='Post effect' key=1 help='Toggle post effect mode.' ");

	TwAddSeparator(bar, "settings object", "");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation, " label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &light_direction, " label='Light direction' opened=true help='Change the light direction.' ");

	TwAddSeparator(bar, "program", "");
	TwAddButton(bar, "Exit", &exitCallbackTw, nullptr, "");
}

void ComputePos(int unused)
{
	cam->updatePos();
	glutTimerFunc(10, ComputePos, 0);
}

#pragma region event_callback
void KeyboardDown(unsigned char key, int x, int y)
{
	if (key == ' ')
	{
		mode_ui = !mode_ui;
		if (mode_ui)
		{
			glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
			glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
			glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
			glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
		}
		else
		{
			glutMouseFunc(MouseButton);
			glutMotionFunc(MouseMove);
			glutPassiveMotionFunc(NULL);
			glutSpecialFunc(NULL);
		}
	}
	if (!mode_ui)
	{
		switch (key)
		{
		case 'z':
			cam->deltaForward = -1;
			break;
		case 's':
			cam->deltaForward = 1;
			break;
		case 'q':
			cam->deltaStrafe = -1;
			break;
		case 'd':
			cam->deltaStrafe = 1;
			break;
		}
	}
	TwDraw();
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int xx, int yy)
{
	if (!mode_ui)
	{
		switch (key)
		{
		case 'z':
		case 's':
			cam->deltaForward = 0;
			break;
		case 'q':
		case 'd':
			cam->deltaStrafe = 0;
			break;
		}
	}
}

void MouseButton(int button, int state, int x, int y)
{
	if (!mode_ui)
	{
		// Gestion camera en fonction du clic souris
		if (button == GLUT_RIGHT_BUTTON)
		{
			// Relacher la camera
			if (state == GLUT_UP)
			{
				cam->releaseCam();
			}
			// Mise à jour origine du clic
			else
			{
				cam->grabCam(x, y);
			}
		}
	}
	glutPostRedisplay();
}

void MouseMove(int x, int y)
{
	if (!mode_ui)
		cam->orienterCam(x, y);

	glutPostRedisplay();
}

void Terminate()
{
	TwTerminate();
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

void TW_CALL SetAliasingCB(const void *value, void *clientData)
{
	(void)clientData;
	aliasing = *(const int *)value;
}
void TW_CALL GetAliasingCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = aliasing;
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
	glutLeaveMainLoop();
}

#pragma endregion