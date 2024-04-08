struct VertexOutput {
    float4 potision : SV_POSITION;
    float2 texcoord : TEXCOORD;
    //float3 normal : NORMAL;
    float4 pxcolor : COLOR;
};

struct InstancingStruct {
    float4x4 WVP;
    float4 color;
};
Texture2D<float4> gTexture : register(t0);
StructuredBuffer<InstancingStruct> gInstancingStruct : register(t1);

