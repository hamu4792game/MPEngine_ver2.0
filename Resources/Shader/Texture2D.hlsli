struct VertexOutput
{
    float4 potision : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct TransformationMatrix
{
    float4x4 WVP;
};