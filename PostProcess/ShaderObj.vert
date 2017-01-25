#version 150

in vec3 a_position;
in vec2 a_textCoords;
in vec3 a_normals;

uniform mat4 VP;
uniform mat4 M;
uniform int flipText;

out vec2 v_textCoords;
out vec3 normals;

void main()
{
	v_textCoords.x = a_textCoords.x;
	if(flipText == 1)
		v_textCoords.y = a_textCoords.y * -1;
	else 
		v_textCoords.y = a_textCoords.y;
	mat3 normalMatrix = mat3(M);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);
	normals = normalMatrix * a_normals;
    gl_Position = VP * M * vec4(a_position, 1.0f) ; 
}