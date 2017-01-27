#version 150

in vec3 position;
in vec2 textCoord;
in vec3 normal;

uniform mat4 viewproj_matrix;
uniform mat4 model_matrix;

out vec3 pos;
out vec2 textCoords;
out vec3 normals;
out vec3 viewVector;

uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
void main()
{
	normals = mat3(model_matrix) * normal;
	vec3 viewspace = mat3(model_matrix) * position;
	viewVector = -viewspace.xyz;

	textCoords = textCoord;

    pos = vec3(viewproj_matrix * model_matrix * vec4(position, 1.0f)); 
    gl_Position = viewproj_matrix * model_matrix * vec4(position, 1.0f); 
}