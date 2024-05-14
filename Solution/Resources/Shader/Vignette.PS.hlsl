#include "Fullscreen.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

float Vignetting(float2 texcoord) {
    // 周囲を0に、中心になるほど明るくなるように計算で調整
    float2 correct = texcoord * (1.0f - texcoord.yx);
    // correctだけで計算すると中心の最大値が0.0625で暗すぎるのでscaleで調整。この例では16バイして1にしている
    float vignette = correct.x * correct.y * 16.0f;
    // とりあえず0.8乗
    vignette = saturate(pow(vignette,0.8f));
    return vignette;
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler,input.texcoord);
    float3 vignetteColor = {0.1f,0.0f,0.0f};

    // 係数として乗算
    float vignette = Vignetting(input.texcoord);
    output.color.rgb *= vignette;
    output.color.rgb += (1.0f - vignette) * vignetteColor;
    return output;
}