#version 450 core
layout (location = 0) out int gEntity;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedoSpec;
layout (binding = 0) uniform sampler2D u_Textures[32];

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform float u_TilingFactor[32];
const int TYPE_TEXTURE_MAXCOUNT = 4;
uniform int u_texture_Diffuse[TYPE_TEXTURE_MAXCOUNT];
uniform int u_texture_Specular[TYPE_TEXTURE_MAXCOUNT];
uniform int uEntity;

vec4 GetTexColor(int index){
	vec4 texColor = vec4(1.0,1.0,1.0,1.0);
	
	switch(index)
	{
		case  0: texColor *= texture(u_Textures[ 0], TexCoords * u_TilingFactor[index]); break;
		case  1: texColor *= texture(u_Textures[ 1], TexCoords * u_TilingFactor[index]); break;
		case  2: texColor *= texture(u_Textures[ 2], TexCoords * u_TilingFactor[index]); break;
		case  3: texColor *= texture(u_Textures[ 3], TexCoords * u_TilingFactor[index]); break;
		case  4: texColor *= texture(u_Textures[ 4], TexCoords * u_TilingFactor[index]); break;
		case  5: texColor *= texture(u_Textures[ 5], TexCoords * u_TilingFactor[index]); break;
		case  6: texColor *= texture(u_Textures[ 6], TexCoords * u_TilingFactor[index]); break;
		case  7: texColor *= texture(u_Textures[ 7], TexCoords * u_TilingFactor[index]); break;
		case  8: texColor *= texture(u_Textures[ 8], TexCoords * u_TilingFactor[index]); break;
		case  9: texColor *= texture(u_Textures[ 9], TexCoords * u_TilingFactor[index]); break;
		case 10: texColor *= texture(u_Textures[10], TexCoords * u_TilingFactor[index]); break;
		case 11: texColor *= texture(u_Textures[11], TexCoords * u_TilingFactor[index]); break;
		case 12: texColor *= texture(u_Textures[12], TexCoords * u_TilingFactor[index]); break;
		case 13: texColor *= texture(u_Textures[13], TexCoords * u_TilingFactor[index]); break;
		case 14: texColor *= texture(u_Textures[14], TexCoords * u_TilingFactor[index]); break;
		case 15: texColor *= texture(u_Textures[15], TexCoords * u_TilingFactor[index]); break;
		case 16: texColor *= texture(u_Textures[16], TexCoords * u_TilingFactor[index]); break;
		case 17: texColor *= texture(u_Textures[17], TexCoords * u_TilingFactor[index]); break;
		case 18: texColor *= texture(u_Textures[18], TexCoords * u_TilingFactor[index]); break;
		case 19: texColor *= texture(u_Textures[19], TexCoords * u_TilingFactor[index]); break;
		case 20: texColor *= texture(u_Textures[20], TexCoords * u_TilingFactor[index]); break;
		case 21: texColor *= texture(u_Textures[21], TexCoords * u_TilingFactor[index]); break;
		case 22: texColor *= texture(u_Textures[22], TexCoords * u_TilingFactor[index]); break;
		case 23: texColor *= texture(u_Textures[23], TexCoords * u_TilingFactor[index]); break;
		case 24: texColor *= texture(u_Textures[24], TexCoords * u_TilingFactor[index]); break;
		case 25: texColor *= texture(u_Textures[25], TexCoords * u_TilingFactor[index]); break;
		case 26: texColor *= texture(u_Textures[26], TexCoords * u_TilingFactor[index]); break;
		case 27: texColor *= texture(u_Textures[27], TexCoords * u_TilingFactor[index]); break;
		case 28: texColor *= texture(u_Textures[28], TexCoords * u_TilingFactor[index]); break;
		case 29: texColor *= texture(u_Textures[29], TexCoords * u_TilingFactor[index]); break;
		case 30: texColor *= texture(u_Textures[30], TexCoords * u_TilingFactor[index]); break;
		case 31: texColor *= texture(u_Textures[31], TexCoords * u_TilingFactor[index]); break;
		default: texColor *= 0.0;break;
	}
	return texColor;
}
void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(FragPos,1.0);
    // also store the per-fragment normals into the gbuffer
	float a =GetTexColor(u_texture_Diffuse[0]).a;
	if(a == 0.0)  discard;
    gNormal = vec4(normalize(Normal),a);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = GetTexColor(u_texture_Diffuse[0]).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = 1.0;
	gEntity = uEntity;
}