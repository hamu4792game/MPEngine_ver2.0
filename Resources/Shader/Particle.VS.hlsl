#include "Particle.hlsli"

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD, uint32_t instanceId : SV_InstanceID) {
    VertexOutput output;
    output.potision = mul(pos, gInstancingStruct[instanceId].WVP);
    output.texcoord = uv;
    //output.normal = normalize(mul(input.normal,(float3)gTransformationMatrices[instanceId].World));
    output.pxcolor = gInstancingStruct[instanceId].color;
	return output;
}
