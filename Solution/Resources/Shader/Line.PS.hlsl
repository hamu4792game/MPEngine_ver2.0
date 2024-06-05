#include "Line.hlsli"

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = input.color;
    
    return textureColor;
}