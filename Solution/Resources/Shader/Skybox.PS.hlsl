#include "Skybox.hlsli"

TextureCube<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Material : register(b1) {
    float32_t4 color;
};

float32_t4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * color;
    

    return textureColor;
}