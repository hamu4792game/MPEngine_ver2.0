#include "Particle.hlsli"

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t1);

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD, uint32_t instanceId : SV_InstanceID)
{
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrices[instanceId].WVP);
    output.texcoord = uv;
    //output.normal = normalize(mul(input.normal,(float3)gTransformationMatrices[instanceId].World));
	return output;
}
