#include "Line.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexOutput main(float4 pos : POSITION)
{
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrix.WVP);
	return output;
}
