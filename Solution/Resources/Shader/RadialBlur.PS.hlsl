#include "Fullscreen.hlsli"
Texture2D<float32_t4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct Parameter {
    float32_t2 center;
    float32_t blurWidth;
};
ConstantBuffer<Parameter> gParameter : register(b0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    
    const float32_t2 kCenter = float32_t2(0.5f,0.5f); // 中心点、ここを基準に放射状ブラーがかかる
    const int32_t kNumSamples = 10; // サンプリング数。多いほど滑らかだが重い
    const float32_t kBlurWidth = -0.01f; // ぼかしの幅。大きいほど大きい
    // 中心から現在のUVに対しての方向を計算
    // 普段方向といえば、単位ベクトルだが、ここではあえて正規化せず、遠いほどより遠くをサンプリングする
    float32_t2 direction = input.texcoord - gParameter.center;
    // 画面外に出ないようにするため、方向の制限
    direction = clamp(direction, float32_t2(-0.5f, -0.5f), float32_t2(0.5f, 0.5f));
    float32_t3 outputColor = float32_t3(0.0f,0.0f,0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        // 現在のUVからさきほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float32_t2 texcoord = input.texcoord + direction * gParameter.blurWidth * float32_t(sampleIndex);
        // texcoordの範囲内にクランプ、ラップを防ぐため
        texcoord = clamp(texcoord, float32_t2(0.0f, 0.0f), float32_t2(1.0f, 1.0f));
        outputColor.rgb += gTexture.Sample(gSampler,texcoord).rgb;
    }
    // 平均化する
    outputColor.rgb *= rcp(kNumSamples);

    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}