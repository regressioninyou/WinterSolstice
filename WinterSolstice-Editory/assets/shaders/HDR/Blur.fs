#version 450 core
layout (location = 0) out vec4 FragColor0;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D HDR;
uniform int uOutLevel;
vec3 sampleHDRTexture(vec2 uv, vec2 textureSize) {
    ivec2 texelCoords = ivec2(uv * textureSize); // 将纹理坐标转换为纹理坐标

    // 采样当前位置的像素值以及其上下左右四个位置的像素值
    vec3 colorCenter = texelFetch(HDR, texelCoords, 0).rgb;
    vec3 colorUp = texelFetch(HDR, texelCoords + ivec2(0, 1), 0).rgb;
    vec3 colorDown = texelFetch(HDR, texelCoords + ivec2(0, -1), 0).rgb;
    vec3 colorLeft = texelFetch(HDR, texelCoords + ivec2(-1, 0), 0).rgb;
    vec3 colorRight = texelFetch(HDR, texelCoords + ivec2(1, 0), 0).rgb;

    // 将四个位置的像素值相加并除以4，得到新的像素值
    vec3 averagedColor = (colorCenter + colorUp + colorDown + colorLeft + colorRight) / 5.0;

    return averagedColor;
}

vec2 getTextureSize(sampler2D textureSampler) {
    return textureSize(textureSampler, 0);
}
void main()
{             
	vec2 TexSize = getTextureSize(HDR);
    vec4 outColor = vec4(sampleHDRTexture(TexCoords,TexSize),1.0);

	FragColor0 = outColor;
}
