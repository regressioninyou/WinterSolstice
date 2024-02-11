
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;
uniform mat4 uViewProjection;

const int MAX_BONES = 200;
uniform mat4 gBones[MAX_BONES];
uniform mat4 uModel;
out vec2 vTexcoord;
void main (){
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];
	vTexcoord = aTexcoord;
    vec4 PosL = BoneTransform * vec4( aPosition + (aNormal * 0.0001) ,1.0f); 
	gl_Position = uViewProjection * uModel * PosL;
}