#include "Fullscreen.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler,input.texcoord);
    float32_t value = dot(output.color.rgb,float32_t3(0.2125f,0.7154f,0.0721f));
    //output.color.rgb = value * float32_t3(1.0f,74.0f / 107.0f,43.0f / 107.0f);
    output.color.rgb = value * float32_t3(0.9f,0.7f,0.4f);
    return output;
}