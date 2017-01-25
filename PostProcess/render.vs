#version 150

in vec3 position;
in vec2 textCoord;
in vec3 normal;

uniform mat4 viewproj_matrix;
uniform mat4 model_matrix;

out vec2 v_textCoords;
out vec3 normals;

void main()
{
	v_textCoords = textCoord;
	mat3 normalMatrix = mat3(model_matrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);
	normals = normalMatrix * normal;
    gl_Position = viewproj_matrix * model_matrix * vec4(position, 1.0f) ; 
}