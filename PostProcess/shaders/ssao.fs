#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=1) uniform sampler2D texNormal;
layout(binding=2) uniform sampler2D textureDepth;
layout(binding=3) uniform sampler2D textureNoise;

uniform int width;
uniform int height;
uniform vec3 kernel[64];
uniform int kernelSize;
uniform mat4 u_invProjectionMatrix;

out vec4 color;
uniform int texNormActive;

in vec2 v_texCoords;
in vec3 v_normal;
in mat3 v_normalMatrix;
in mat4 v_projectionMatrix;

float radius = 0.5;

vec3 positionFromDepth(vec2 texCoord)
{
    float z = texture(textureDepth, texCoord).r;  
	float x = texCoord.x * 2.0 - 1.0;
    float y = texCoord.y * 2.0 - 1.0;

    vec4 vProjectedPos = vec4(x, y, z, 1.0);
    vec4 vPositionVS = /*u_invProjectionMatrix **/  vProjectedPos;  
    return vPositionVS.xyz / vPositionVS.w;  
}

vec3 calculate_occlusion()
{
	vec3 origin = positionFromDepth(v_texCoords.st);
	//return vec3(origin);
	vec3 normal = texture2D(texNormal, v_texCoords).rgb;

	vec2 noiseScale = vec2(width / 4, height / 4);
	vec3 rvec = texture2D(textureNoise, v_texCoords * noiseScale).xyz;

	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; ++i) 
	{
		// get sample position:
	   vec3 sample = tbn * kernel[i];
	   sample = origin + sample * radius;
		
		// project sample position:
	   vec4 offset = vec4(sample, 1.0);
	   offset = v_projectionMatrix * offset;
	   offset.xyz /= offset.w;
	   offset.xyz = offset.xyz * 0.5 + 0.5;
		
		// get sample depth:
	   vec3 pos = positionFromDepth(offset.xy);
	   float sampleDepth = pos.z;
  
		// range check & accumulate:
	   // float rangeCheck= abs(origin.z - sampleDepth) < radius ? 1.0 : 0.0;
	   float bias = 0.025;
	   occlusion += sampleDepth;(sampleDepth >= sample.z + bias? 1.0 : 0.0);// * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	return vec3(occlusion, occlusion, occlusion);
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
	vec3 colorOut = calculate_occlusion();
	color = vec4(vec3(colorOut), 1);
}