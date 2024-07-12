#include "Fullscreen.hlsli"
Texture2D<float32_t4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct Parameter {
    float32_t grayscalevalue;
};
ConstantBuffer<Parameter> gParameter : register(b0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler,input.texcoord);
    float32_t value = dot(output.color.rgb,float32_t3(0.2125f,0.7154f,0.0721f));
    float32_t3 grayscale = float32_t3(value,value,value) * gParameter.grayscalevalue + output.color.rgb * (1.0f - gParameter.grayscalevalue);
    output.color.rgb = float32_t3(value,value,value);
    output.color.a = 1.0f;
    return output;
}