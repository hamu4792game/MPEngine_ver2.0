#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Material : register(b1) {
    float4 color;
    int enableLighting;
    float shininess;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

float4 main(VertexOutput input) : SV_TARGET {
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * color;
    float3 diffuse;

    if (enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        diffuse = 
        textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        //float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float3 reflectLight = reflect(gDirectionalLight.direction,normalize(input.normal));
        //float NDotH = dot(normalize(input.normal),halfVector);
        float RdotE = dot(reflectLight,toEye);
        float specularPow = pow(saturate(RdotE),shininess); // 反射強度
        float3 specular = 
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f,1.0f,1.0f);

        textureColor.rgb = diffuse + specular;
    }
    
    if (textureColor.a == 0.0) {
        discard;
    }

    return textureColor;
}