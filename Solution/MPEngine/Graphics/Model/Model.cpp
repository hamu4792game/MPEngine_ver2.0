#include "Model.h"
decltype(Model::modelLists_) Model::modelLists_;

Model::Model() {
	modelLists_.emplace_back(this);
}

Model::~Model() {
	modelLists_.remove(this);
}

void Model::SetModel(Object3d* model) {
	model_ = model;
	texture_ = model->GetTexture();
}

