#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=1) uniform sampler2D textureColor;
out vec4 color;
in vec2 v_texCoords;

uniform int width;
uniform int height;

#define FxaaInt2 ivec2
#define FxaaFloat2 vec2
uniform float FXAA_SUBPIX_SHIFT = 1.0/4.0;
#define FXAA_REDUCE_MUL   (1.0/8.0)
#define FXAA_SPAN_MAX     8.0
#define FxaaTexOff(t, p, o, r) texture2DLodOffset(t, p, 0.0, o)

vec3 FxaaPixelShader(vec4 posPos, sampler2D tex, vec2 rcpFrame)
{   
    #define FXAA_REDUCE_MIN   (1.0/128.0)

    vec3 rgbNW = texture2D(tex, posPos.zw, 0.0).xyz;
    vec3 rgbNE = FxaaTexOff(tex, posPos.zw, FxaaInt2(1,0), rcpFrame.xy).xyz;
    vec3 rgbSW = FxaaTexOff(tex, posPos.zw, FxaaInt2(0,1), rcpFrame.xy).xyz;
    vec3 rgbSE = FxaaTexOff(tex, posPos.zw, FxaaInt2(1,1), rcpFrame.xy).xyz;
    vec3 rgbM  = texture2D(tex, posPos.xy, 0.0).xyz;

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

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(FxaaFloat2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
          max(FxaaFloat2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), 
          dir * rcpDirMin)) * rcpFrame.xy;

    vec3 rgbA = (1.0/2.0) * (
        texture2D(tex, posPos.xy + dir * (1.0/3.0 - 0.5), 0.0).xyz +
        texture2D(tex, posPos.xy + dir * (2.0/3.0 - 0.5), 0.0).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        texture2D(tex, posPos.xy + dir * (0.0/3.0 - 0.5), 0.0).xyz +
        texture2D(tex, posPos.xy + dir * (3.0/3.0 - 0.5), 0.0).xyz);
    float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB; 
}

vec3 aliasing()
{
  vec2 rcpFrame = vec2(1.0/width, 1.0/height);
  vec4 posPos;
  posPos.xy = v_texCoords.xy;
  posPos.zw = v_texCoords.xy - 
                  (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));

  vec3 color = FxaaPixelShader(posPos, textureColor, rcpFrame);
  return color;
}

void main()
{
	vec3 result = aliasing();
	color = vec4(result.rgb,1.0);

	color = texture(textureColor, v_texCoords);
}