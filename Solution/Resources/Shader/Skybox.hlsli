struct VertexOutput {
    float32_t4 potision : SV_POSITION;
    float32_t3 texcoord : TEXCOORD0;
};

struct TransformationMatrix {
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};
