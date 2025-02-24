#include "Fullscreen.hlsli"
Texture2D<float32_t4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t grayscale = textureColor.r * 0.299f + textureColor.g * 0.587f + textureColor.b * 0.114f;
    // エルミート補間を返す
    float32_t extract = smoothstep(0.6f,0.9f,grayscale);

    output.color = textureColor * extract;
    //output.color.a = 1.0f;

    return output;
}
