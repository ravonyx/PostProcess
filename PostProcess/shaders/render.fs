#version 150

out vec4 color[4];

in vec3 pos;
in vec3 normals;
in vec3 viewVector;

uniform vec3 light_direction;
uniform vec3 cam_position;
uniform vec3 diffuse_albedo = vec3(0.8, 0.8, 0.9);
uniform vec3 specular_albedo = vec3(0.1);
uniform float specular_power = 42.0;
uniform float shading_level = 1.0;

void main()
{
	vec3 normal_calc = normalize(normals);
    vec3 light_direction_norm = normalize(light_direction);
	vec3 view_direction = normalize(-cam_position);

    // Calculate R locally
    vec3 R = reflect(-light_direction_norm, normal_calc);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(normal_calc, light_direction_norm), 0.0) * diffuse_albedo;
    diffuse *= diffuse;

	vec3 vert_to_eye = normalize(cam_position - pos);
	float specular_factor = dot(vert_to_eye, -R);
	vec3 specular = vec3(0,0,0);
	if (specular_factor > 0) 
	{
		specular_factor = pow(specular_factor, specular_power);
		specular = specular_albedo* specular_factor;
	}

    // Write final color to the framebuffer
    color[0] = mix(vec4(0.0), vec4(diffuse + specular, 1.0), shading_level);
    color[1] = vec4(vec3(normals), viewVector.z);
    color[2] = vec4(vec3(specular_factor), 1.0);
}