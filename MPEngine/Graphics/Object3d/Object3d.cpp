#include "Object3d.h"
//#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"

Object3d::~Object3d() {
	if (resource_) {
		resource_->Release();
		resource_.Reset();
	}
}

void Object3d::Load(const std::string& name, const std::string& filePath) {
	name_ = name;
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	// モデル読み込み
	modelData_ = rsManager->LoadObjFile(filePath);

	texture_ = std::make_shared<Texture>();
	texture_->Load(modelData_.material.textureFilePath);
	rsManager->AddTexture(name_ + "Texture", texture_);

}
