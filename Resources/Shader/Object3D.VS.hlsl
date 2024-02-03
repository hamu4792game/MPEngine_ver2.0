#include "Object3D.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD, float3 normal : NORMAL0) {
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrix.WVP);
    output.texcoord = uv;
    output.normal = normalize(mul(normal,(float3x3)gTransformationMatrix.World));
	return output;
}
