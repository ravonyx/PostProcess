#version 150

in vec2 a_position;
out vec2 v_texCoords;

void main()
{
	v_texCoords = (a_position + 1.0) / 2.0;
    gl_Position = vec4(a_position, 0.0f, 1.0f); 
}