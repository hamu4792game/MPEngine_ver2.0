// SkinnningObject3d.VS.hlslで作ったものと同じPalette
struct Well {
    float4x4 skeletonSpaceMatrix;
    float4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t0);
// VertexBufferViewのstream0として利用していた入力頂点
struct Vertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};
StructuredBuffer<Vertex> gInputVertices : register(t1);
// VertexBufferViewのstream1として利用していた入力インフルエンス
struct VertexInfluence {
    float32_t4 weight;
    int32_t4 index;
};
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// Skinnning計算後の頂点データ、SkinningVertex
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
// Skinninngに関するちょっとした情報
struct SkinningInfomation {
    uint32_t numVertices;
};
ConstantBuffer<SkinningInfomation> gSkinningInfomation : register(b0);

[numthreads(1024,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    uint32_t vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInfomation.numVertices) {
        // skinningの計算処理
        // 必要なデータをStructuredBufferから取ってくる
        // SkinningObject3d.VSでは入力頂点として受け取っていた
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];

        // skinning後の頂点を計算
        Vertex skinned;
        skinned.texcoord = input.texcoord;
        
        // 位置の変換
        skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
        skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
        skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
        skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
        skinned.position.w = 1.0f; // 確実に1を入れる
        // 法線の変換
        skinned.normal = mul(input.normal, (float3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
        skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
        skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
        skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
        skinned.normal = normalize(skinned.normal); // 正規化して戻してあげる
        
        // Skinning後の頂点データを格納、つまり書き込む
        gOutputVertices[vertexIndex] = skinned;
    }
}