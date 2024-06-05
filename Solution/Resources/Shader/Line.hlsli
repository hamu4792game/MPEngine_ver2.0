struct VertexOutput {
    float4 potision : SV_POSITION;
    float4 color : COLOR;
};

struct InstancingStruct {
    float4x4 WVP;
    float4 color;
};
StructuredBuffer<InstancingStruct> gInstancingStruct : register(t0);