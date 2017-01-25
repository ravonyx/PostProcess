#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D texture_data;

out vec4 color;
in vec2 v_texCoords;

float LinearizeDepth(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 1000.0; // camera z far
  float z = texture2D(texture_data, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main()
{
	color = vec4(vec3(LinearizeDepth(v_texCoords)), 1);
}


