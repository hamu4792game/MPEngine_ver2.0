struct VertexOutput {
    float4 potision : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct TransformationMatrix {
    float4x4 WVP;
};