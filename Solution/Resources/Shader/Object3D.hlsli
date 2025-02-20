struct VertexOutput {
    float4 potision : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    float32_t4 scale : SCALE0;
};

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

struct DirectionalLight {
	float4 color;
	float3 direction;
	float intensity;
};

struct Camera {
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b3);