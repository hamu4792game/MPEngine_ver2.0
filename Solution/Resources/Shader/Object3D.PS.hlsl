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
    float4x4 uvMat;
};

ConstantBuffer<Material> gMaterial : register(b1);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

float4 main(VertexOutput input) : SV_TARGET {
    float4 uv = mul(float4(input.texcoord,0.0f,1.0f),gMaterial.uvMat);
    // uvTranslateが1.0だった場合になる特殊フラグ これでタイリングを行う 絶対後で消す
    float4x4 uvMatrix;
    if (true) {
        // 長い成分探し
        uint32_t index = 0;
        for (int i = 0; i < 3; i++) {
            if (input.normal[i] != 0.0f) { 
                index = i;
                break;
            }
        }
        // 二次元+1次元ベクトルを作る
        float32_t3 sVec = float32_t3(gMaterial.uvMat[0][0],gMaterial.uvMat[1][1],gMaterial.uvMat[2][2]);
        if (i != 2) {
            sVec[i] = sVec.z;
        }
        sVec.z = 1.0f;
        sVec.x = sVec.x * 2.0f;
        sVec.y = sVec.y * 2.0f;
        sVec = 1.0f / sVec;
        uvMatrix = float4x4(
            sVec.x,       0.0f,         0.0f,         0.0f,
            0.0f,         sVec.y,       0.0f,         0.0f,
            0.0f,         0.0f,         sVec.z,       0.0f,
            0.0f,         0.0f,         0.0f,         1.0f
        );
        uv = mul(float4(input.texcoord,0.0f,1.0f),uvMatrix);
    }

    // 2Dテクスチャのためにxy成分のみを使用
    float2 uv2D = (uv.xy) - floor((uv.xy));
    float4 textureColor = gTexture.Sample(gSampler, uv2D) * gMaterial.color;
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

    if (gMaterial.uvMat[3][0] >= 1.0f && gMaterial.uvMat[3][1] >= 1.0f && gMaterial.uvMat[3][2] >= 1.0f) {
        textureColor.rgb = float3(0.0f,0.0f,0.0f);
    }

    if (textureColor.a == 0.0) {
        discard;
    }

    return textureColor;
}