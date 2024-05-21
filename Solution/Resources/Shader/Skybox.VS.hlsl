#include "Skybox.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexOutput main(float32_t4 pos : POSITION, float32_t3 uv : TEXCOORD) {
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrix.WVP).xyww;
    output.texcoord = pos.xyz;
    return output;
}
