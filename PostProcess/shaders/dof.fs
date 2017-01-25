#version 140

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_texture_multisample : enable
#extension GL_EXT_gpu_shader4 : enable

layout(binding=0) uniform sampler2D textureColor;
layout(binding=1) uniform sampler2D textureDepth;
out vec4 color;

in vec2 v_texCoords;
in mat4 v_projectionMatrix;


float LinearizeDepth(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 1000.0; // camera z far
  float z = texture2D(textureDepth, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

float Convolve(float[9] kernel, float[9] matrix, float denom, float offset)
{
   float res = 0.0;
   for (int i=0; i<9; i++) {
      res += kernel[i]*matrix[i];
   }
   return clamp(res/denom + offset,0.0,1.0);
}

float[9] GetData(int channel)
{
   float dxtex = 1.0 / float(textureSize(textureColor,0));  
   float dytex = 1.0 / float(textureSize(textureColor,0));
   float[9] mat;
   int k = -1;
   for (int i=-1; i<2; i++) 
   {   
      for(int j=-1; j<2; j++) 
	  {    
         k++;    
		 float dx = float(i)*dxtex;
		 float dy = float(j)*dytex;
         mat[k] =  texture2D(textureColor,v_texCoords.xy + vec2(dx, dy))[channel]; 
      }
   }
   return mat;
}

void main()
{
	float matr[9] = GetData(0);
	float matg[9] = GetData(1);
	float matb[9] = GetData(2);
	float[9] gaussBlur = float[]  (1.,2.,1., 2., 4., 2., 1., 2., 1.);

	float z = vec3(LinearizeDepth(v_texCoords.xy)).r;
	vec4 result;
	if(z > 0.2)
		result = vec4(Convolve(gaussBlur,matr,16.,0.), Convolve(gaussBlur,matg,16.,0.),Convolve(gaussBlur,matb,16.,0.),1.0);
	else
		result = texture(textureColor, v_texCoords);
	
	color = vec4(result.rgb,1.0);
}