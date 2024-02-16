#version 450 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if (hdr)
    {
        vec3 result = hdrColor * exposure;
        FragColor = vec4(result, 1.0);
    }
    else
    {
        FragColor = vec4(hdrColor, 1.0);
    }
}
