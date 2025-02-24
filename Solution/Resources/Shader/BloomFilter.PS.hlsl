#include "Fullscreen.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
Texture2D<float4> gBlurTexture : register(t0); // ぼかしテクスチャ
SamplerState gSampler : register(s0); // サンプラー

#define MAX_TEXTURE_NUM 2

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    output.color = gTexture.Sample(gSampler,input.uv);
    output.color += gBlurTexture.Sample(gSampler,input.uv);
    output.color / MAX_TEXTURE_NUM;
    output.clor.a = 1.0f;

    return output;
}