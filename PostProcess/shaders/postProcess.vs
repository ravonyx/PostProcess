#version 150

#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec2 a_position;
layout(location=1) in vec2 a_textCoords;
layout(location=2) uniform mat4 u_projectionMatrix;

out vec2 v_texCoords;
out mat4 v_projectionMatrix;

void main()
{
	v_texCoords = (a_position + 1.0) / 2.0;
	v_projectionMatrix = u_projectionMatrix;
    gl_Position = vec4(a_position, 0.0f, 1.0f); 
}