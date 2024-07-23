#include "Texture2D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Material : register(b1) {
    float4 color;
    float4x4 uvMat;
};


float4 main(VertexOutput input) : SV_TARGET {
    float4 uv = mul(float4(input.texcoord,0.0f,1.0f),uvMat);
    float4 textureColor = gTexture.Sample(gSampler, uv.xy) * color;
    if (textureColor.a < 0.01f) {
        discard;
    }
    return textureColor;
}