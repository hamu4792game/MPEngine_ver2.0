#include "Line.hlsli"

cbuffer Iro : register(b1) {
    float4 color;
};

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = color;
    
    return textureColor;
}