#version 430 core

layout (binding = 0) uniform sampler2D colorTexture;

in vec2 v_texCoords;

// Final output
layout (location = 0) out vec4 color;

// Various uniforms controling antiAliasing Effect
uniform float width; 
uniform float height; 
uniform float FXAA_SPAN_MAX = 8.0;
uniform float FXAA_REDUCE_MUL = 1.0/8.0;
#define FXAA_REDUCE_MIN   (1.0/128.0)
uniform float FXAA_SUBPIX_SHIFT = 1.0/4.0;

vec3 antiAliasing()
{   
	vec2 frame = vec2(1.0/width, 1.0/height);
	vec4 posPos;
	posPos.xy = v_texCoords.xy;
	posPos.zw = v_texCoords.xy - (frame * (0.5 + FXAA_SUBPIX_SHIFT));

    vec3 rgbNW = textureLod(colorTexture, posPos.zw, 0.0).xyz;
    vec3 rgbNE = textureLodOffset(colorTexture, posPos.zw, 0.0, ivec2(1,0)).xyz;
    vec3 rgbSW = textureLodOffset(colorTexture, posPos.zw, 0.0, ivec2(0,1)).xyz;
    vec3 rgbSE = textureLodOffset(colorTexture, posPos.zw, 0.0, ivec2(1,1)).xyz;
    vec3 rgbM  = textureLod(colorTexture, posPos.xy, 0.0).xyz;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir; 
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), 
          dir * rcpDirMin)) * frame.xy;

    vec3 rgbA = (1.0/2.0) * (
        textureLod(colorTexture, posPos.xy + dir * (1.0/3.0 - 0.5), 0.0).xyz +
        textureLod(colorTexture, posPos.xy + dir * (2.0/3.0 - 0.5), 0.0).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        textureLod(colorTexture, posPos.xy + dir * (0.0/3.0 - 0.5), 0.0).xyz +
        textureLod(colorTexture, posPos.xy + dir * (3.0/3.0 - 0.5), 0.0).xyz);

    float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) 
		return rgbA;
    return rgbB; 
}

    
void main() 
{ 
	color = vec4(antiAliasing(), 1.0);
}