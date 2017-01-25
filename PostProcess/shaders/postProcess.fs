#version 140
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D textureColor;
layout(binding=1) uniform sampler2D textureDepth;
uniform int width;
uniform int height;
out vec4 color;
in vec2 v_texCoords;
in mat4 v_projectionMatrix;

vec4 black_white()
{
	vec4 sum = vec4(0.0);

	sum = texture(textureColor, v_texCoords);
	float luminance = (sum.r + sum.g + sum.b) /3;
	sum.r = sum.g = sum.b = luminance;
	sum.a = 1;
	return sum;
}


float LinearizeDepth(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 1000.0; // camera z far
  float z = texture2D(textureDepth, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main()
{
	color = black_white();
	//color = texture(textureColor, v_texCoords);
}
