#include "peraPolygon.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

cbuffer EffectParameters : register(b0) {
    float2 centerPosition;
    float parameterRate;
    int type;
}

// ハッシュ関数
float hash( float n )
{
    return frac(sin(n)*43758.5453);
}

// 3次元ベクトルからシンプレックスノイズを生成する関数
float SimplexNoise(float3 x)
{
    // The noise function returns a value in the range -1.0f -> 1.0f
    float3 p = floor(x);
    float3 f = frac(x);

    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
                     lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
                lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                     lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

float4 AverageBlur(Output input,float radius) {
    float4 sum =  gTexture.Sample(gSampler,input.uv);
    float offsetU = radius / 1280.0f;
    float offsetV = radius / 720.0f;

    sum += gTexture.Sample(gSampler,input.uv + float2(offsetU,0.0f));
    sum += gTexture.Sample(gSampler,input.uv + float2(-offsetU,0.0f));
    sum += gTexture.Sample(gSampler,input.uv + float2(0.0f,offsetV));
    sum += gTexture.Sample(gSampler,input.uv + float2(0.0f,-offsetV));
    sum += gTexture.Sample(gSampler,input.uv + float2(offsetU,offsetV));
    sum += gTexture.Sample(gSampler,input.uv + float2(offsetU,-offsetV));
    sum += gTexture.Sample(gSampler,input.uv + float2(-offsetU,offsetV));
    sum += gTexture.Sample(gSampler,input.uv + float2(-offsetU,-offsetV));

    return sum /= 9.0f;
}

//  ピクセルシェーダー
float4 main(Output input) : SV_Target{
    float4 textureColor = gTexture.Sample(gSampler, input.uv);
    float4 result;
    float Y;
    int levels = 8;
    
    switch(type) {
    case 1: // グレースケール
        Y = (textureColor.x + textureColor.y + textureColor.z) / 3.0f;
        result = float4(Y,Y,Y,textureColor.a);
        break;
    case 2: // レトロ調
        result = float4(textureColor.rgb - fmod(textureColor.rgb, 0.25f),textureColor.a);
        break;
    case 3: // セピア調
        Y = (textureColor.x + textureColor.y + textureColor.z) / 3.0f;
        result = float4(Y * 0.9f,Y * 0.7f,Y * 0.4f,textureColor.a);
        break;
    case 4: // ネガポジ反転
        result.rgb = float3(1.0f - textureColor.r,1.0f - textureColor.g,1.0f - textureColor.b);
        break;
    case 5: // ノイズフィルター
        Y = SimplexNoise(input.position.xyz);
        Y = (Y - 0.5f) * 2.0f;
        result = gTexture.Sample(gSampler,input.uv + Y * 0.01f);
        break;
    case 6: // ポスタライズフィルター
        result.rgb = round(textureColor.rgb * levels) / levels;
        result.a = textureColor.a;
        break;
    case 7: // 色の置き換え
        result.r = textureColor.g;
        result.g = textureColor.b;
        result.b = textureColor.r;
        break;
    case 8: // 色の置き換え
        result.r = textureColor.b;
        result.g = textureColor.r;
        result.b = textureColor.g;
        break;
    case 9: // 平均化ブラー 
        result = AverageBlur(input,1.0f);
        result.a = textureColor.a;
        break;
    default:
        result = textureColor;
        break;
    }

    // 円の衝突判定
    float v = pow(input.position.x - centerPosition.x,2) + 
    pow(input.position.y - centerPosition.y,2);
    if (v <= pow(parameterRate,2)) {
        return result;
    }

    // ひし形の衝突判定
    /*float v = abs(input.position.x - centerPosition.x)
     + abs(input.position.y - centerPosition.y);
    if (v <= parameterRate) {
        return result;
    }*/

    return textureColor;
}

