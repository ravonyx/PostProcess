#version 150

#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec4 a_position;
layout(location=0) uniform mat4 u_worldMatrix;

layout(location=1) uniform mat4 u_viewMatrix;
layout(location=2) uniform mat4 u_projectionMatrix;

void main(void)
{
	vec4 positionWS = u_worldMatrix * a_position;
	gl_Position = u_projectionMatrix * u_viewMatrix * positionWS;
}
