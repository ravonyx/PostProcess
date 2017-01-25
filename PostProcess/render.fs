#version 130

out vec4 color;

in vec2 textCoords;
in vec3 normals;
in vec3 lightVector;
in vec3 viewVector;

uniform vec3 diffuse_albedo = vec3(0.8, 0.8, 0.9);
uniform vec3 specular_albedo = vec3(0.01);
uniform float specular_power = 128.0;
uniform float shading_level = 1.0;

void main()
{
	color = vec4(vec3(normals), 1);

	vec3 normalCalc = normalize(normals);
    vec3 lightVectorCalc = normalize(lightVector);
    vec3 viewVectorCalc = normalize(viewVector);

    // Calculate R locally
    vec3 R = reflect(-lightVectorCalc, normalCalc);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(normalCalc, lightVectorCalc), 0.0) * diffuse_albedo;
    diffuse *= diffuse;
    vec3 specular = pow(max(dot(R, viewVectorCalc), 0.0), specular_power) * specular_albedo;

    // Write final color to the framebuffer
    //color = mix(vec4(0.0), vec4(diffuse + specular, 1.0), shading_level);

	color = vec4(vec3(normals), 1.0);
   // normal_depth = vec4(normalCalc, viewVector.z);
}