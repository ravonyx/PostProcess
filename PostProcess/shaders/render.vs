#version 150

in vec3 position;
in vec2 textCoord;
in vec3 normal;

uniform mat4 viewproj_matrix;
uniform mat4 model_matrix;

out vec2 textCoords;
out vec3 normals;
out vec3 lightVector;
out vec3 viewVector;

uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
void main()
{
	mat3 normalMatrix = mat3(model_matrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);

	normals = normalMatrix * normal;
	textCoords = textCoord;

    gl_Position = viewproj_matrix * model_matrix * vec4(position, 1.0f) ; 
}