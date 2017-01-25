#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D texture_data;

out vec4 color;
in vec2 v_texCoords;

void main()
{
	color = texture(texture_data, v_texCoords);
}


