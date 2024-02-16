#version 450 core
layout (location = 0) out vec4 FragColor0;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D HDR[32];
uniform int BlurLevel;

void main()
{             
	vec4 outColor = vec4(0.0);
	for(int i = 0; i < BlurLevel;i++){
		outColor += texture(HDR[i],TexCoords);
	}
	FragColor0 = clamp(outColor, 0.0, 1.0);
}
