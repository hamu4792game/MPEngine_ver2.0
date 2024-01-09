#include "Particle.hlsli"

SamplerState gSampler : register(s0);

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * input.pxcolor;
    
    if(textureColor.a == 0.0f) {
        discard;
    }
    
    return textureColor;
}