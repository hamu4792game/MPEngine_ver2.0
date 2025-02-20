#include "Object3D.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD, float3 normal : NORMAL0) {
    VertexOutput output;
    output.potision = mul(pos, gTransformationMatrix.WVP);
    output.texcoord = uv;
    output.normal = normalize(mul(normal,(float3x3)gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(pos,gTransformationMatrix.World).xyz;

    //scale
    output.scale.x = sqrt(pow(gTransformationMatrix.World[0][0],2) + pow(gTransformationMatrix.World[0][1],2) + pow(gTransformationMatrix.World[0][2],2));
    output.scale.y = sqrt(pow(gTransformationMatrix.World[1][0],2) + pow(gTransformationMatrix.World[1][1],2) + pow(gTransformationMatrix.World[1][2],2));
    output.scale.z = sqrt(pow(gTransformationMatrix.World[2][0],2) + pow(gTransformationMatrix.World[2][1],2) + pow(gTransformationMatrix.World[2][2],2));
    output.scale.w = 1.0f;
    return output;
}
