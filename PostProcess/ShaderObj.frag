#version 130

in vec2 v_textCoords;
in vec3 normals;

uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 specular;
uniform vec3 camPos;
uniform float shininess;

uniform sampler2D texDiffuse;

uniform int material;
uniform int objOnly;
uniform int useTexDiffuse;

uniform int useNormals;
uniform float light_multiplier;
uniform vec3 light_dir;

vec3 normalCalc;
out vec4 outColor;

void main()
{
	vec4 texD = texture2D(texDiffuse, v_textCoords);
	normalCalc = normalize(normals);

		
	outColor = vec4(vec3(normals), 1);
	//if(useTexDiffuse == 1)
	//	outColor = (Iambient + Idiff + Ispec) * texD;
	//else
	//	outColor = Iambient + Idiff + Ispec;
}