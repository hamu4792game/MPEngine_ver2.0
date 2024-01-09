#include "PlayerParticle.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "externals/imgui/imgui.h"

PlayerParticle::PlayerParticle() {
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
}

void PlayerParticle::Initialize(const Vector3& emitter) {
	
}

void PlayerParticle::Update(const Vector3& emitter) {

	coolTime_++;
	// 初期化生成
	for (uint32_t index = 0u; index < kNumCount_; index++) {
		if (!particleparam_.at(index).isAlive_ && (coolTime_ >= 4.0f)) {
			StateInitialize(emitter, index);
			coolTime_ = 0.0f;
			break;
		}
	}

	int count = 0;
	// 更新
	for (uint32_t index = 0u; index < kNumCount_; index++) {
		if (!particleparam_.at(index).isAlive_) {
			continue;
		}
		count++;
		particleparam_.at(index).transform_.translation_ += particleparam_.at(index).velocity_ * particleparam_.at(index).speed_;
		particleparam_.at(index).color_ -= Vector4(0.0f, 0.0f, 0.0f, 0.01f);
		if (particleparam_.at(index).color_.w <= 0.0f) {
			particleparam_.at(index).isAlive_ = false;
		}

		// 入れ子
		particle_->transform_.at(index) = particleparam_.at(index).transform_;
		particle_->color_.at(index) = particleparam_.at(index).color_;
	}
	ImGui::Text("%d", count);

}

void PlayerParticle::StateInitialize(const Vector3& emitter,const uint32_t& num) {
	Vector3 colorRand = RandNum(Vector3::zero, Vector3::one);
	particleparam_.at(num).color_ = Vector4(colorRand.x, colorRand.y, colorRand.z, 1.0f);
	particleparam_.at(num).isAlive_ = true;

	if (!flag_) {
		Vector3 randTrans = RandNum(Vector3(-2.0f, 0.0f, -2.0f), Vector3(2.0f, 0.0f, 2.0f));
		particleparam_.at(num).transform_.translation_ = emitter + randTrans;
		particleparam_.at(num).transform_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
		particleparam_.at(num).velocity_ = Normalize(RandNum(Vector3(0.0f, 0.1f, 0.0f), Vector3(0.0f, 0.1f, 0.0f)));
		particleparam_.at(num).speed_ = 0.1f;
	}
	else {
		particleparam_.at(num).transform_.translation_ = emitter;
		particleparam_.at(num).transform_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
		particleparam_.at(num).velocity_ = Normalize(RandNum(-Vector3::one, Vector3::one));
	}


}
