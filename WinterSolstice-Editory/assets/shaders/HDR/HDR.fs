#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 v_Bloom;

in vec2 TexCoords;

layout (binding = 0)uniform sampler2D hdrBuffer;
layout (binding = 1)uniform sampler2D BloomBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec4 bloom = texture(BloomBuffer,TexCoords);
    if(hdr)
    {
        // reinhard
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
        if(bloom.a > 0.0){
            v_Bloom = vec4(result,bloom.a);
        }
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);

        if(bloom.a > 0.0){
            vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
            // also gamma correct while we're at it       
            result = pow(result, vec3(1.0 / gamma));
            v_Bloom = vec4(result,bloom.a);
        }
    }
}