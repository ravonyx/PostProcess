#version 150

out vec4 color[4];

in vec2 textCoords;
in vec3 normals;
in vec3 lightVector;
in vec3 viewVector;

uniform vec3 light_direction;
uniform vec3 cam_position;
uniform vec3 diffuse_albedo = vec3(0.8, 0.8, 0.9);
uniform vec3 specular_albedo = vec3(0.01);
uniform float specular_power = 128.0;
uniform float shading_level = 1.0;

void main()
{
	//color = vec4(vec3(normals), 1);

	vec3 normal_calc = normalize(normals);
    vec3 light_direction_norm = normalize(light_direction);
	vec3 view_direction = normalize(-cam_position);

    // Calculate R locally
    vec3 R = reflect(-light_direction_norm, normal_calc);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(normal_calc, light_direction_norm), 0.0) * diffuse_albedo;
    diffuse *= diffuse;
    vec3 specular = pow(max(dot(R, view_direction), 0.0), specular_power) * specular_albedo;

    // Write final color to the framebuffer
    color[0] = mix(vec4(0.0), vec4(diffuse + specular, 1.0), shading_level);
    color[1] = vec4(vec3(normals), 1.0);

	//color = vec4(vec3(light_direction_norm), 1.0);
	//color = vec4(vec3(normals), 1.0);
   // normal_depth = vec4(normal_calc, viewVector.z);
}