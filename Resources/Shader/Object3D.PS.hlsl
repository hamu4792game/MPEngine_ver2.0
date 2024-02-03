#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Material : register(b1) {
    float4 color;
    int enableLighting;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * color;

    if (enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        textureColor.rgb = 
        textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    
    return textureColor;
}