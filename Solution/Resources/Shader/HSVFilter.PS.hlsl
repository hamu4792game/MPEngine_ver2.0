#include "Fullscreen.hlsli"
Texture2D<float4> gTexture : register(t0); // 通常テクスチャ
SamplerState gSampler : register(s0); // サンプラー

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};
struct Material {
    float32_t hue;
    float32_t saturation;
    float32_t value;
};
ConstantBuffer<Material> gMaterial : register(b0);

float32_t3 RGBToHSV(float32_t3 rgb) {
    float32_t3 hsv;
    float lMax = 0;
    lMax = rgb.r > rgb.g ? rgb.r : rgb.g;
    lMax = lMax > rgb.b ? lMax : rgb.b;
    float lMin = 0;
    lMin = rgb.r < rgb.g ? rgb.r : rgb.g;
    lMin = lMin < rgb.b ? lMin : rgb.b;
    hsv.x = lMax - lMin;
    if (hsv.x > 0.0f) {
        if (lMax == rgb.r) {
            hsv.x = (rgb.g - rgb.b) / hsv.x;
        }
        else if(lMax == rgb.g) {
            hsv.x = 2.0f + (rgb.b - rgb.r) / hsv.x;
        }
        else if(lMax == rgb.b) {
            hsv.x = 4.0f + (rgb.r - rgb.g) / hsv.x;
        }
    }
    hsv.x /= 6.0f;

    hsv.y = lMax - lMin;
    if (lMax != 0.0f) {
        hsv.y /= lMax;
    }
    
    hsv.z = lMax;
    return hsv;
};

float32_t3 HSVToRGB(float32_t3 hsv) {
    float32_t3 rgb = float32_t3(hsv.z,hsv.z,hsv.z);
    if (hsv.y > 0.0f) {
        hsv.x *= 6.0f;
        const int i = (int)hsv.x;
        const float f = hsv.x - (float)i;
        switch(i) {
            default:
            case 0:
                rgb.g *= 1.0f - hsv.y * (1.0f - f);
                rgb.b *= 1.0f - hsv.y;
                break;
            case 1:
                rgb.r *= 1.0f - hsv.y * f;
                rgb.b *= 1.0f - hsv.y;
                break;    
            case 2:
                rgb.r *= 1.0f - hsv.y;
                rgb.b *= 1.0f - hsv.y * (1.0f - f);
                break;    
            case 3:
                rgb.r *= 1.0f - hsv.y;
                rgb.g *= 1.0f - hsv.y * f;
                break;
            case 4:
                rgb.r *= 1.0f - hsv.y * (1.0f - f);
                rgb.g *= 1.0f - hsv.y;
                break;    
            case 5:
                rgb.g *= 1.0f - hsv.y;
                rgb.b *= 1.0f - hsv.y * f;
                break;
        }
    }
    return rgb;
};

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange) {
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange,range);
    if (modValue < 0.0f) {
        modValue += range;
    }
    return minRange + modValue;
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler,input.texcoord);
    float32_t3 hsv = RGBToHSV(output.color.rgb);

    //各パラメータの増減
    hsv.x += gMaterial.hue;
    hsv.y += gMaterial.saturation;
    hsv.z += gMaterial.value;

    // 制限
    hsv.x = WrapValue(hsv.x,0.0f,1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);

    float32_t3 rgb = HSVToRGB(hsv);

    output.color.rgb = rgb;
    return output;
}