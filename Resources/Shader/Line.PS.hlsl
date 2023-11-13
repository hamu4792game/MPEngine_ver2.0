#include "Line.hlsli"

//Texture2D<float4> gTexture : register(t0);
//SamplerState gSampler : register(s0);

cbuffer Iro : register(b1)
{
    float4 color;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = color;
    
    return textureColor;
}