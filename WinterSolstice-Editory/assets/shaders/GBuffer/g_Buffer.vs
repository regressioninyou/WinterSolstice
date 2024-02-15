#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;

const int MAX_BONES = 200;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uViewProjection;

layout(std140, binding = 0) uniform BonesBlock {
    mat4 gBones[MAX_BONES];
};

void main()
{
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];

    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexcoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    Normal = normalMatrix * aNormal;

    gl_Position = uViewProjection * worldPos; 
}
