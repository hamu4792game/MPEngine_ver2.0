#include "Texture2D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Iro : register(b1)
{
    float4 color;
};


cbuffer Monochrome : register(b2)
{
    float2 monochromePibot;
    float monochromeRate;
}

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * color;
    //  YUBの色を取り、輝度値を取り出す。(0.299f,0.587f,0.114f)これでYの値を取得
    //float Y = dot(textureColor.xyz, float3(0.299f, 0.587f, 0.114f));
    //  従来のグレースケール計算
    /*
    float Y = (textureColor.x + textureColor.y + textureColor.z) / 3.0f;
    float3 monochrome = { Y, Y, Y };
    
    float len = monochromePibot.x - input.potision.x;
    len = clamp(len, 0.0f, 1.0f);
    textureColor.xyz = lerp(textureColor.xyz, monochrome, len);
    */

    float2 posCenter = input.potision.xy - monochromePibot;
    clip(monochromeRate - length(posCenter));
    
    return textureColor;
}