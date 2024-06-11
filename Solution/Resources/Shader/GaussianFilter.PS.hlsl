#include "Fullscreen.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma) {
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    // expはネイピア数eの累乗を求める関数
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input) {
    // kernelを求める
    uint32_t width,height;
    gTexture.GetDimensions(width,height);
    float32_t2 uvStepSize = float32_t2(rcp(width),rcp(height));
    
    PixelShaderOutput output;
    output.color.rgb = float32_t3(0.0f,0.0f,0.0f);
    output.color.a = 1.0f;

    float weight = 0.0f;
    const uint kMaxSize = 5u; // 最大サイズ
    float halfArraySize = float(kMaxSize - 1u) * 0.5f;
    float kernelArray[kMaxSize][kMaxSize];

    const float32_t a = 2.0f; // 標準辺差a、のちに定数で渡す
    // kernelを求める
    for (int32_t i = 0; i < kMaxSize; ++i) {
        for(int32_t j = 0; j < kMaxSize; ++j) {
            kernelArray[i][j] = gauss(i - halfArraySize, j - halfArraySize, a);
            weight += kernelArray[i][j];

            // 求めたkernelを使い、BoxFilterと同じく畳み込みを行う
            float32_t2 texcoord = input.texcoord + float32_t2(i - halfArraySize, j - halfArraySize) * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler,texcoord).rgb;
            output.color.rgb += fetchColor * kernelArray[i][j];
        }
    }

    output.color.rgb *= rcp(weight);

    return output;
}