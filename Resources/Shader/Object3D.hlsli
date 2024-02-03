struct VertexOutput {
    float4 potision : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 World;
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