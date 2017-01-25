#pragma once
#include "EsgiShader.h"
#include "Camera.h"
#include "Obj.h"

int programBasicID, programSkyboxID, programObjID, programObjEnvMappingID, programPostID, programGridID;
EsgiShader basicShader, skyboxShader, objShader, objShaderEnvMapping, postShader, gridShader;

Camera *cam;
Obj	model;

int post_effect, env_mapping;
float ratio, angle;

float scale;
Quaternion rotation;

float light_multiplier;
glm::vec3 light_direction;