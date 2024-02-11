#version 430

layout(local_size_x = 1, local_size_y = 1) in;

uniform sampler2D textureSampler; // 输入纹理
uniform int texWidth; // 纹理宽度
uniform int texHeight; // 纹理高度

layout(std430, binding = 0) buffer ResultBuffer {
    int result; // 用于存储检查结果的缓冲
};

void main() {
    int numTransparentPixels = 0;

    // 遍历纹理中的像素
    for (int y = 0; y < texHeight; ++y) {
        for (int x = 0; x < texWidth; ++x) {
            vec4 color = texture(textureSampler, vec2(x / float(texWidth), y / float(texHeight)));
            // 检查像素的 alpha 值
            if (color.a < 1.0) {
                numTransparentPixels++;
            }
        }
    }

    // 如果有半透明像素，则结果为1，否则为0
    result = numTransparentPixels > 0 ? 1 : 0;
}
