#version 450 core
precision highp float;
layout (location = 0) out vec4 FragColor;

#define PI 3.14159265359;
layout (binding = 0)uniform sampler2D inputTexture;
uniform vec2 imageSize; // 图像尺寸

vec2 multiplyComplex(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 addComplex(vec2 a, vec2 b) {
    return a + b;
}

vec2 subtractComplex(vec2 a, vec2 b) {
    return a - b;
}

vec2 twiddleFactor(int i, int N) {
    float angle = -2.0 * PI * float(i) / float(N);
    return vec2(cos(angle), sin(angle));
}

void main() {
    vec2 texCoord = gl_FragCoord.xy / imageSize;

    // 从输入纹理中读取数据，假设纹理包含灰度值
    float intensity = texture2D(inputTexture, texCoord).r;

    // 定义FFT变换尺寸
    int N = int(imageSize.x);

    // 计算傅里叶变换
    vec2 sum = vec2(0.0);
    for (int k = 0; k < N; ++k) {
        vec2 twiddle = twiddleFactor(k, N);
        vec2 inputSample = vec2(intensity, 0.0); // 输入信号假设为实数部分
        vec2 exponent = vec2(0.0);
        for (int n = 0; n < N; ++n) {
            vec2 tw = multiplyComplex(twiddle, vec2(0.0, float(n)));
            exponent = addComplex(exponent, tw);
            vec2 twSample = multiplyComplex(inputSample, vec2(cos(exponent.y), sin(exponent.y)));
            sum = addComplex(sum, twSample);
        }
    }

    // 将结果写入到输出颜色
    FragColor = vec4(length(sum), 0.0, 0.0, 1.0); // 输出频域的模值
}
