#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D texture_data;

out vec4 color;
in vec2 v_texCoords;
 
uniform mat4 screenToView;
uniform vec3 focus;

void main(void)
{
	float depth = texture(texture_data, v_texCoords).r;

	vec2  xy = v_texCoords;
	vec4  wViewPos =  screenToView * vec4(xy, depth, 1.0);
	vec3  viewPos = vec3(wViewPos/wViewPos.w);
	float viewDepth = viewPos.z;

	if(viewDepth < focus.x)
		color = vec4( clamp( abs( (viewDepth - focus.x) / focus.y ), 0.0, 1.0), 0.0, 0.0, 1.0 );
	else
		color = vec4( clamp( abs( (viewDepth - focus.x) / focus.z ), 0.0, 1.0), 0.0, 0.0, 1.0 );
}