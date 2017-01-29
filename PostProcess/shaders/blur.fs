#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D texture_data;

out vec4 color;
in vec2 v_texCoords;
 
uniform ivec2 direction;
uniform float resolution = 500;
uniform float radius = 0.8;

void main(void)
{
    vec4 sum = vec4(0.0);
    float blur = radius/resolution; 

    float hstep = direction.x;
    float vstep = direction.y;

    //9filter with predefined gaussian weights

    sum += texture2D(texture_data, vec2(v_texCoords.x - 4.0*blur*hstep, v_texCoords.y - 4.0*blur*vstep)) * 0.0162162162;
    sum += texture2D(texture_data, vec2(v_texCoords.x - 3.0*blur*hstep, v_texCoords.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture2D(texture_data, vec2(v_texCoords.x - 2.0*blur*hstep, v_texCoords.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture2D(texture_data, vec2(v_texCoords.x - 1.0*blur*hstep, v_texCoords.y - 1.0*blur*vstep)) * 0.1945945946;

    sum += texture2D(texture_data, vec2(v_texCoords.x, v_texCoords.y)) * 0.2270270270;

    sum += texture2D(texture_data, vec2(v_texCoords.x + 1.0*blur*hstep, v_texCoords.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture2D(texture_data, vec2(v_texCoords.x + 2.0*blur*hstep, v_texCoords.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture2D(texture_data, vec2(v_texCoords.x + 3.0*blur*hstep, v_texCoords.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture2D(texture_data, vec2(v_texCoords.x + 4.0*blur*hstep, v_texCoords.y + 4.0*blur*vstep)) * 0.0162162162;

    color = vec4(sum.rgb, 1.0);
}