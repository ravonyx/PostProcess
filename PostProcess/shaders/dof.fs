#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D colorTexture;
layout(binding=1) uniform sampler2D blurTexture;
layout(binding=2) uniform sampler2D cocTexture;

out vec4 color;
in vec2 v_texCoords;

void main()
{
	vec4 colorTex = texture(colorTexture, v_texCoords);
	vec4 blurTex = texture(blurTexture, v_texCoords);
	float coef = texture(cocTexture, v_texCoords).x;
	color = mix(colorTex, blurTex, coef);
}


