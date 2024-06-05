#include "Line.hlsli"

static const uint32_t kNumVertex = 2u;
static const float32_t4 kPositions[kNumVertex] = {
    {0.0f,0.0f,0.0f,1.0f},
    {1.0f,0.0f,0.0f,1.0f}
};

VertexOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceId : SV_InstanceID) {
    VertexOutput output;
    output.potision = mul(kPositions[vertexID], gInstancingStruct[instanceId].WVP);
    output.color = gInstancingStruct[instanceId].color;
	return output;
}
