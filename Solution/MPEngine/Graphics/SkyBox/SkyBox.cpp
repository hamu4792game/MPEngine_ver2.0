#include "SkyBox.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
decltype(SkyBox::skyBoxLists_) SkyBox::skyBoxLists_;

const uint32_t kMaxVertex = 8u;

SkyBox::SkyBox() {
	skyBoxLists_.emplace_back(this);
	CreateVertexResource();
	CreateIndexResource();
	UploadVertexData();
}

SkyBox::~SkyBox() {
	skyBoxLists_.remove(this);
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	if (indexResource_) {
		indexResource_->Release();
		indexResource_.Reset();
	}
}

void SkyBox::SetTexture(Texture* texture) {
	texture_ = texture;
	uvScale_ = Vector2(1.0f, 1.0f);
}

void SkyBox::CreateVertexResource() {
	// 頂点データ
	if (!vertexResource_) {
		vertexResource_ = ResourceManager::GetInstance()->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(VertexData) * kMaxVertex);
	}

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * kMaxVertex;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void SkyBox::CreateIndexResource() {
	// index情報を作る
	// 1.indexの情報を送るためにmap用のuint16_t型の配列を作る(中身は頂点の組み合わせ、要素番号)
	//uint16_t indices[6] = { 0,1,3,1,2,3 };
	std::array<uint16_t, 36u> indices = {
		0,1,2,2,1,3,// 右
		4,5,6,6,5,7,// 左
		8,9,10,10,9,11,// 前
		12,13,14,14,13,15,// 奥
		16,17,18,18,17,19,// 上
		20,21,22,22,21,23,// 下
	};
	// 2.Resourceを生成
	if (!indexResource_) {
		indexResource_ = ResourceManager::GetInstance()->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(indices));
	}
	// 3.indexBufferViewを生成
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.Format = DXGI_FORMAT_R16_UINT;
	indexBufferView_.SizeInBytes = sizeof(indices);

	// 4.GPUに送るためのMap用のuint16_tポインタを生成
	uint16_t* indexData = nullptr;

	// 5.resoureceのMap関数を呼んで関連付けさせる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	// 6.4で作ったものに3のものをコピー
	//std::copy(std::begin(indices), std::end(indices), indexData);
	std::memcpy(indexData, indices.data(), sizeof(uint16_t) * indices.size());
	// 7.Unmapする
	indexResource_->Unmap(0, nullptr);
}

void SkyBox::UploadVertexData() {
	// 頂点データ
	// SkyBoxではtexcoordもnormalも使わないので、コンストラクタでの初期化
	std::array<VertexData, kMaxVertex> vertices;
	// 右面。描画インデックスは[0,1,2][2,1,3]で内面を向く
	vertices[0].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 右上奥
	vertices[1].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f); // 右上前
	vertices[2].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f); // 右下奥
	vertices[3].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f); // 右下前
	// 左面。描画インデックスは[4,5,6][6,5,7]
	vertices[4].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f); // 左上前
	vertices[5].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f); // 左上奥
	vertices[6].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f); // 左下前
	vertices[7].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f); // 左上前
	/*// 前面。描画インデックスは[8,9,10][10,9,11]
	vertices[8].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f); // 左上奥
	vertices[9].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 右上奥
	vertices[10].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f); // 左下前
	vertices[11].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f); // 右下前
	// 後面。描画インデックスは[12,13,14][14,13,15]
	vertices[12].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f); // 右上前
	vertices[13].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f); // 左上前
	vertices[14].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f); // 右下前
	vertices[15].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f); // 左下前
	// 上面。描画インデックスは[16,17,18][18,17,19]
	vertices[16].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f); // 左上前
	vertices[17].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f); // 右上前
	vertices[18].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f); // 左上奥
	vertices[19].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 右上奥
	// 下面。描画インデックスは[20,21,22][22,21,23]
	vertices[20].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f); // 左下奥
	vertices[21].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f); // 右下奥
	vertices[22].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f); // 左下前
	vertices[23].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f); // 右下前
	*/

	VertexData* mapData = nullptr;

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::copy(std::begin(vertices), std::end(vertices), mapData);
	// 重要
	vertexResource_->Unmap(0, nullptr);

}