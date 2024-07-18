#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);

struct Material {
    float4 color;
    int enableLighting;
    float shininess;
    int phongLighing;
    float32_t environmentCoefficient;
};

ConstantBuffer<Material> gMaterial : register(b1);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * gMaterial.color;
    float3 diffuse;

    if (gMaterial.enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        diffuse = 
        textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        textureColor.rgb = diffuse;

        if (gMaterial.phongLighing != 0) {
            float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            //float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float3 reflectLight = reflect(gDirectionalLight.direction,normalize(input.normal));
            //float NDotH = dot(normalize(input.normal),halfVector);
            float RdotE = dot(reflectLight,toEye);
            float specularPow = pow(saturate(RdotE),gMaterial.shininess); // 反射強度
            float3 specular = 
            gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f,1.0f,1.0f);
            textureColor.rgb += specular;
        }
    }
    
    // 環境マップ
    {
        float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float32_t3 reflectedVector = reflect(cameraToPosition,normalize(input.normal));
        float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler,reflectedVector);
        textureColor.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
    }

    if (textureColor.a == 0.0) {
        discard;
    }

    return textureColor;
}