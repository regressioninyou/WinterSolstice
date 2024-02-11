#version 450 core
layout(location = 0) out vec4 color;
layout (binding = 0) uniform sampler2D u_Textures[32];

uniform float u_TilingFactor[32];
const int TYPE_TEXTURE_MAXCOUNT = 4;
uniform int u_texture_Diffuse[TYPE_TEXTURE_MAXCOUNT];
uniform int u_texture_Specular[TYPE_TEXTURE_MAXCOUNT];
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexcoord;
flat in ivec4 vBoneIDs;
in vec4 vWeights;
flat in mat4 vBoneTransform;
vec4 GetTexColor(int index){
	vec4 texColor = vec4(1.0,1.0,1.0,1.0);
	
	switch(index)
	{
		case  0: texColor *= texture(u_Textures[ 0], vTexcoord * u_TilingFactor[index]); break;
		case  1: texColor *= texture(u_Textures[ 1], vTexcoord * u_TilingFactor[index]); break;
		case  2: texColor *= texture(u_Textures[ 2], vTexcoord * u_TilingFactor[index]); break;
		case  3: texColor *= texture(u_Textures[ 3], vTexcoord * u_TilingFactor[index]); break;
		case  4: texColor *= texture(u_Textures[ 4], vTexcoord * u_TilingFactor[index]); break;
		case  5: texColor *= texture(u_Textures[ 5], vTexcoord * u_TilingFactor[index]); break;
		case  6: texColor *= texture(u_Textures[ 6], vTexcoord * u_TilingFactor[index]); break;
		case  7: texColor *= texture(u_Textures[ 7], vTexcoord * u_TilingFactor[index]); break;
		case  8: texColor *= texture(u_Textures[ 8], vTexcoord * u_TilingFactor[index]); break;
		case  9: texColor *= texture(u_Textures[ 9], vTexcoord * u_TilingFactor[index]); break;
		case 10: texColor *= texture(u_Textures[10], vTexcoord * u_TilingFactor[index]); break;
		case 11: texColor *= texture(u_Textures[11], vTexcoord * u_TilingFactor[index]); break;
		case 12: texColor *= texture(u_Textures[12], vTexcoord * u_TilingFactor[index]); break;
		case 13: texColor *= texture(u_Textures[13], vTexcoord * u_TilingFactor[index]); break;
		case 14: texColor *= texture(u_Textures[14], vTexcoord * u_TilingFactor[index]); break;
		case 15: texColor *= texture(u_Textures[15], vTexcoord * u_TilingFactor[index]); break;
		case 16: texColor *= texture(u_Textures[16], vTexcoord * u_TilingFactor[index]); break;
		case 17: texColor *= texture(u_Textures[17], vTexcoord * u_TilingFactor[index]); break;
		case 18: texColor *= texture(u_Textures[18], vTexcoord * u_TilingFactor[index]); break;
		case 19: texColor *= texture(u_Textures[19], vTexcoord * u_TilingFactor[index]); break;
		case 20: texColor *= texture(u_Textures[20], vTexcoord * u_TilingFactor[index]); break;
		case 21: texColor *= texture(u_Textures[21], vTexcoord * u_TilingFactor[index]); break;
		case 22: texColor *= texture(u_Textures[22], vTexcoord * u_TilingFactor[index]); break;
		case 23: texColor *= texture(u_Textures[23], vTexcoord * u_TilingFactor[index]); break;
		case 24: texColor *= texture(u_Textures[24], vTexcoord * u_TilingFactor[index]); break;
		case 25: texColor *= texture(u_Textures[25], vTexcoord * u_TilingFactor[index]); break;
		case 26: texColor *= texture(u_Textures[26], vTexcoord * u_TilingFactor[index]); break;
		case 27: texColor *= texture(u_Textures[27], vTexcoord * u_TilingFactor[index]); break;
		case 28: texColor *= texture(u_Textures[28], vTexcoord * u_TilingFactor[index]); break;
		case 29: texColor *= texture(u_Textures[29], vTexcoord * u_TilingFactor[index]); break;
		case 30: texColor *= texture(u_Textures[30], vTexcoord * u_TilingFactor[index]); break;
		case 31: texColor *= texture(u_Textures[31], vTexcoord * u_TilingFactor[index]); break;
		default: texColor *= 0.0;break;
	}
	return texColor;
}
void main(){
vec3 lightPos = vec3(10.0,0.0,10.0);
vec3 pos = vec3(0.0,0.0,0.0);
vec3 lightDir = normalize(lightPos - vPosition);
float diff = max(dot(lightDir,vNormal),0.5);
vec4 specular = GetTexColor(u_texture_Specular[0]);
vec4 diffuse = GetTexColor(u_texture_Diffuse[0]);
float wight = 1.0;
vec4 m = specular + diffuse;
if(m.a == 0.0) 
	discard;
m.xyz *= diff;
color = m;
}