#include "Model.h"
decltype(Model::modelLists_) Model::modelLists_;

Model::Model() {
	modelLists_.emplace_back(this);
	cMaterial->uvMatrix = MakeIdentity4x4();
}

Model::~Model() {
	modelLists_.remove(this);
}

void Model::SetModel(Object3d* model) {
	model_ = model;
	texture_ = model->GetTexture();
}

void Model::SetTexture(Texture* texture, const int32_t number) {
	// マテリアルより多ければ何もせず早期リターン
	if (texture_.size() < number) { return; }
	// -1だったら全部同じtextureに
	if (number == -1) {
		for (auto& tex : texture_) {
			tex = texture;
		}
		return;
	}
	// 違ったらその番号に代入
	texture_.at(number) = texture;
}
