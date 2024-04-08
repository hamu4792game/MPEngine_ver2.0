#include "Texture2D.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrix.WVP);
    output.texcoord = uv;
	return output;
}
