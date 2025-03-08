// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 430
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	int map_Kd;
};

uniform int ch;

uniform MaterialInfo Material;
out vec4 vFragColor;
uniform sampler2D BodyTex;
uniform sampler2D EyesTex;
uniform sampler2D ZipTex;
uniform sampler2D toungeTex;

//lighting color
vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(0.8,0.8,0.8,1);   
vec4    specularColor = vec4(1,1,1,1);
int nbins = 6;
in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec2 UV;
float Shininess = 128.0;//for material specular

uniform float time;

vec2 resolution=vec2(800.0,600.0);

vec3 hsv2rgb(vec3 c)
     {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
     }

void main(void)
{ 
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal),
					normalize(vVaryingLightDir)));

    // Multiply intensity by diffuse color, force alpha to 1.0
    vFragColor = diff * diffuseColor*vec4(Material.Kd,1);
	
	//---------texture-
	vec4 color;
	if(Material.map_Kd == 0){
		color = texture(BodyTex, UV).bgra;
	}
	if(Material.map_Kd == 1){
		color = texture(EyesTex, UV).bgra;
	}
	if(Material.map_Kd == 2){
		color = texture(ZipTex, UV).bgra;
	}
	if(Material.map_Kd == 3){
		color = texture(toungeTex, UV).bgra;
	}
    
    //

	if(ch==0)
	{
		ambientColor = vec4(0.329412, 0.223529, 0.027451, 1.000000);
        diffuseColor = vec4(0.780392, 0.568627, 0.113725, 1.000000);   
        specularColor = vec4(0.992157, 0.941176, 0.807843, 1.000000);
	}
	else {
	
	if(ch==2)
	{
	   float gray = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b; 
	   vFragColor = vec4(vec3(gray),1.0);
	}
	else if(ch==1)
	{
	   vFragColor = color;
    }
	else if(ch==3)
	{
		
   
	float sinTime = 1.0 + sin(time / 100.0);
	float sqrln = length(gl_FragCoord.xy - (resolution.xy / 2.0));
	sqrln = pow(sqrln, sinTime);
	float val = sin(sqrln);
	val = 0.5+0.5*(val);
	vec3 rawcol = hsv2rgb(vec3(val,1.0,1.0));

	vFragColor = vec4(rawcol,1.0);
	}
	else if(ch==4)
	{

float r = floor(color.r * float(nbins)) / float(nbins);
float g = floor(color.g * float(nbins)) / float(nbins);
float b = floor(color.b * float(nbins)) / float(nbins);    
vFragColor = vec4(r,g,b,color.a);

	}
    }
	// Add in ambient light
    vFragColor += ambientColor;

    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),
								normalize(vVaryingNormal)));//反射角
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
	if(diff != 0) {
		spec = pow(spec, Shininess);
		vFragColor += specularColor*vec4(Material.Ka,1)*spec;
	}
}
	
    