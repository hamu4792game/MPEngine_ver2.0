#include "DustParticle.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"

DustParticle::DustParticle() {
	auto rsManager = ResourceManager::GetInstance();
	particle_ = std::make_shared<Particle>();
	particle_->SetModel(rsManager->FindObject3d("Plane"), kNumCount_);
	particle_->SetTexture(rsManager->FindTexture("Circle"));
	particle_->SetBlendMode(BlendMode::Add);
	particleparam_.resize(kNumCount_);
	for (uint32_t index = 0u; index < kNumCount_; index++) {
		particle_->transform_.at(index) = particleparam_.at(index).transform_;
		particle_->color_.at(index) = particleparam_.at(index).color_;
	}

	minParam_.transform_.translation_ = Vector3(-2.0f, 0.0f, 0.0f);
	maxParam_.transform_.translation_ = Vector3(2.0f, 0.0f, 0.0f);
	minParam_.transform_.scale_ = Vector3(0.2f, 0.2f, 0.2f);
	maxParam_.transform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);

	minParam_.color_ = Vector4::zero;
	maxParam_.color_ = Vector4::one;

	minParam_.velocity_ = Vector3(0.0f, 0.5f, 0.0f);
	maxParam_.velocity_ = Vector3(0.0f, 2.0f, 0.0f);

	minParam_.speed_ = 0.01f;
	maxParam_.speed_ = 1.0f;

}

void DustParticle::Initialize(const Vector3& emitter) {
	emitter;
}

void DustParticle::Update(const Vector3& emitter) {

	coolTime_++;
	// 初期化生成
	for (uint32_t index = 0u; index < kNumCount_; index++) {
		if (!particleparam_.at(index).isAlive_ && (coolTime_ >= 4.0f)) {
			StateInitialize(emitter, index);
			coolTime_ = 0.0f;
			break;
		}
	}

	// 更新
	for (uint32_t index = 0u; index < kNumCount_; index++) {
		if (!particleparam_.at(index).isAlive_) {
			continue;
		}
		particleparam_.at(index).transform_.translation_ += particleparam_.at(index).velocity_ * particleparam_.at(index).speed_;
		particleparam_.at(index).color_.w -= 0.01f;
		if (particleparam_.at(index).color_.w <= 0.0f) {
			particleparam_.at(index).isAlive_ = false;
		}
		particleparam_.at(index).transform_.UpdateMatrix();

		// 入れ子
		particle_->transform_.at(index) = particleparam_.at(index).transform_;
		particle_->color_.at(index) = particleparam_.at(index).color_;
	}

}

void DustParticle::StateInitialize(const Vector3& emitter, const uint32_t& num) {
	Vector3 colorRand = RandNum(Vector3(minParam_.color_.x, minParam_.color_.y, minParam_.color_.z), Vector3(maxParam_.color_.x, maxParam_.color_.y, maxParam_.color_.z));
	particleparam_.at(num).color_ = Vector4(colorRand.x, colorRand.y, colorRand.z, 1.0f);
	particleparam_.at(num).isAlive_ = true;

	if (!flag_) {
		Vector3 randTrans = RandNum(minParam_.transform_.translation_, maxParam_.transform_.translation_);
		particleparam_.at(num).transform_.translation_ = emitter + randTrans;
		Vector3 randScale = RandNum(minParam_.transform_.scale_, maxParam_.transform_.scale_);
		particleparam_.at(num).transform_.scale_ = randScale;
		particleparam_.at(num).transform_.rotation_.y = AngleToRadian(180.0f);
		particleparam_.at(num).transform_.UpdateMatrix();
		particleparam_.at(num).velocity_ = Normalize(RandNum(minParam_.velocity_, maxParam_.velocity_));
		particleparam_.at(num).speed_ = RandNum(minParam_.speed_, maxParam_.speed_);
	}
	else {
		particleparam_.at(num).transform_.translation_ = emitter;
		particleparam_.at(num).transform_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
		particleparam_.at(num).velocity_ = Normalize(RandNum(-Vector3::one, Vector3::one));
	}


}

void DustParticle::DrawImGui() {
#ifdef _DEBUG
	if (ImGui::TreeNode("MinParameter")) {
		ImGui::DragFloat3("scale", &minParam_.transform_.scale_.x, 0.01f);
		ImGui::DragFloat3("translate", &minParam_.transform_.translation_.x, 0.01f);
		ImGui::DragFloat3("velocity", &minParam_.velocity_.x, 0.01f);
		ImGui::DragFloat4("color", &minParam_.color_.x, 0.01f);
		ImGui::DragFloat("speed", &minParam_.speed_, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("MaXParameter")) {
		ImGui::DragFloat3("scale", &maxParam_.transform_.scale_.x, 0.01f);
		ImGui::DragFloat3("translate", &maxParam_.transform_.translation_.x, 0.01f);
		ImGui::DragFloat3("velocity", &maxParam_.velocity_.x, 0.01f);
		ImGui::DragFloat4("color", &maxParam_.color_.x, 0.01f);
		ImGui::DragFloat("speed", &maxParam_.speed_, 0.01f);
		ImGui::TreePop();
	}

#endif // _DEBUG
}
