#version 450 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D HDR;
layout (binding = 1) uniform sampler2D EmiinseHDR;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(EmiinseHDR, 0); // gets size of single texel
     vec3 result = texture(EmiinseHDR, TexCoords).rgb * weight[0];
     if(texture(EmiinseHDR, TexCoords).a > 0.0 ){
         // 使用单层循环代替双重循环
         for(int i = 1; i < 5; ++i)
         {
             vec2 offset = vec2(tex_offset.x * i, tex_offset.y * i);
             // 水平方向模糊
             result += texture(EmiinseHDR, TexCoords + offset).rgb * weight[i];
             result += texture(EmiinseHDR, TexCoords - offset).rgb * weight[i];
             // 垂直方向模糊
             result += texture(EmiinseHDR, TexCoords + offset.yx).rgb * weight[i];
             result += texture(EmiinseHDR, TexCoords - offset.yx).rgb * weight[i];
         }
     }

     // 从HDR纹理中获取颜色
     vec4 hdr = texture(HDR,TexCoords);
     FragColor = vec4(result, 1.0) * .5 + hdr * .5;
}
