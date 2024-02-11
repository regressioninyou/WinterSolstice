#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;

const int MAX_BONES = 200;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uViewProjection;
out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexcoord;
flat out ivec4 vBoneIDs;
out vec4 vWeights;
flat out mat4 vBoneTransform;
uniform mat4 gBones[MAX_BONES];
void main (){
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];
	vBoneTransform = BoneTransform;
	vTexcoord = aTexcoord;
	vec3 Position = normalize(aPosition);
	vPosition = Position;
	vNormal = aNormal;
	vBoneIDs = aBoneIDs;
	vWeights = aWeights;
	mat4 MVP =  uProjection * uView * uModel;
    vec4 PosL = BoneTransform * vec4(aPosition, 1.0);
	if(PosL.w == 0.0) PosL =  vec4(aPosition, 1.0);
	gl_Position =  uViewProjection * uModel * PosL; 
}