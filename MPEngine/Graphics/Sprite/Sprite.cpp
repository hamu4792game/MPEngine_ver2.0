#include "Sprite.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
decltype(Sprite::spriteLists_) Sprite::spriteLists_;

Sprite::Sprite() {
	spriteLists_.emplace_back(this);
	CreateVertexResource();
}

Sprite::~Sprite() {
	spriteLists_.remove(this);
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	if (indexResource_) {
		indexResource_->Release();
		indexResource_.Reset();
	}
}

void Sprite::SetBlend(BlendMode blend) {
	if (blend == BlendMode::BlendCount) {
		blendType_ = BlendMode::Normal;
		return;
	}
	blendType_ = blend;
}

void Sprite::SetAnchorPoint(AnchorPoint anchor) {
	anchor_ = anchor;
	CreateVertexResource(anchor);
}

void Sprite::CreateVertexResource(AnchorPoint anchor) {
	// 頂点データ
	VertexData vertices[4];
	switch (anchor) {
	case AnchorPoint::Center:
		vertices[0] = { {-0.5f,0.5f,0.1f,1.0f},{0.0f,0.0f} };
		vertices[1] = { {0.5f,0.5f,0.1f,1.0f},{1.0f,0.0f} };
		vertices[2] = { {0.5f,-0.5f,0.1f,1.0f},{1.0f,1.0f} };
		vertices[3] = { {-0.5f,-0.5f,0.1f,1.0f},{0.0f,1.0f} };
		break;
	case AnchorPoint::LeftTop:
		vertices[0] = { {-0.0f,0.0f,0.1f,1.0f},{0.0f,0.0f} };
		vertices[1] = { {1.0f,0.0f,0.1f,1.0f},{1.0f,0.0f} };
		vertices[2] = { {1.0f,-1.0f,0.1f,1.0f},{1.0f,1.0f} };
		vertices[3] = { {-0.0f,-1.0f,0.1f,1.0f},{0.0f,1.0f} };
		break;
	case AnchorPoint::RightTop:
		vertices[0] = { {-1.0f,0.0f,0.1f,1.0f},{0.0f,0.0f} };
		vertices[1] = { {0.0f,0.0f,0.1f,1.0f},{1.0f,0.0f} };
		vertices[2] = { {0.0f,-1.0f,0.1f,1.0f},{1.0f,1.0f} };
		vertices[3] = { {-1.0f,-1.0f,0.1f,1.0f},{0.0f,1.0f} };
		break;
	case AnchorPoint::LeftBottom:
		vertices[0] = { {-0.0f,1.0f,0.1f,1.0f},{0.0f,0.0f} };
		vertices[1] = { {1.0f,1.0f,0.1f,1.0f},{1.0f,0.0f} };
		vertices[2] = { {1.0f,-0.0f,0.1f,1.0f},{1.0f,1.0f} };
		vertices[3] = { {-0.0f,-0.0f,0.1f,1.0f},{0.0f,1.0f} };
		break;
	case AnchorPoint::RightBottom:
		vertices[0] = { {-1.0f,1.0f,0.1f,1.0f},{0.0f,0.0f} };
		vertices[1] = { {0.0f,1.0f,0.1f,1.0f},{1.0f,0.0f} };
		vertices[2] = { {0.0f,-0.0f,0.1f,1.0f},{1.0f,1.0f} };
		vertices[3] = { {-1.0f,-0.0f,0.1f,1.0f},{0.0f,1.0f} };
		break;
	}

	for (uint8_t i = 0; i < 4; i++) {
		vertices[i].normal.x = 0.0f;
		vertices[i].normal.y = 0.0f;
		vertices[i].normal.z = -1.0f;
	}

	vertexResource_ = ResourceManager::GetInstance()->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(vertices));

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(vertices);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//	
	VertexData* mapData = nullptr;

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::copy(std::begin(vertices), std::end(vertices), mapData);
	//for (auto i = 0; i < _countof(vertices); i++)
	//{
	//	mapData[i] = vertices[i];
	//}
	// 重要
	vertexResource_->Unmap(0, nullptr);

	// index情報を作る
	// 1.indexの情報を送るためにmap用のuint16_t型の配列を作る(中身は頂点の組み合わせ、要素番号)
	uint16_t indices[6] = { 0,1,3,1,2,3 };
	// 2.Resourceを生成
	indexResource_ = ResourceManager::GetInstance()->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(indices));
	// 3.indexBufferViewを生成
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.Format = DXGI_FORMAT_R16_UINT;
	indexBufferView_.SizeInBytes = sizeof(indices);

	// 4.GPUに送るためのMap用のuint16_tポインタを生成
	uint16_t* indexData = nullptr;

	// 5.resoureceのMap関数を呼んで関連付けさせる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	// 6.4で作ったものに3のものをコピー
	for (auto i = 0; i < _countof(indices); i++) {
		indexData[i] = indices[i];
	}

	// 7.Unmapする
	indexResource_->Unmap(0, nullptr);
}

