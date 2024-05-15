#include "PlayerAnimation.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#include "PlayerManager.h"

PlayerAnimation::PlayerAnimation(const WorldTransform* transform) {
	auto rsManager = ResourceManager::GetInstance();
	auto global = GlobalVariables::GetInstance();
	global->LoadFile(itemName_);
	// Modelの生成と読み込み
	uint32_t index = 0u;
	for (auto& model : models_) {
		model = std::make_unique<Model>();
		model->SetModel(rsManager->FindObject3d("Human"));
		Vector3 scale = global->GetVector3Value(itemName_, ("PartsTrans : scale" + std::to_string(index)).c_str());
		Vector3 rotation = global->GetVector3Value(itemName_, ("PartsTrans : rotate" + std::to_string(index)).c_str());
		Vector3 translation = global->GetVector3Value(itemName_, ("PartsTrans : translate" + std::to_string(index)).c_str());
		WorldTransform trans = WorldTransform(scale, rotation, translation);
		trans.parent_ = transform;
		model->SetTransform(trans);
		index++;
	}
	// animationの生成と読み込み
	std::string animationNames[static_cast<uint32_t>(AnimationType::kMaxNum)]{
		"HumanWait",
		"HumanWalk",
		"HumanJump",
	};
	index = 0u;
	for (auto& anime : animation_) {
		anime = std::make_unique<ModelAnimation>();
		anime->Load(rsManager->FindAnimation(animationNames[index]), models_.at(static_cast<uint32_t>(Parts::Body)).get());
		index++;
	}
}

void PlayerAnimation::Initialize() {
	animationTime_ = 0.0f;
	nowType_ = AnimationType::Wait;
	oldType_ = AnimationType::kMaxNum;
	models_.at(static_cast<uint32_t>(Parts::Body))->SetAnimation(animation_.at(static_cast<uint32_t>(nowType_)).get());
}

void PlayerAnimation::Update(BehaviorFlag flag) {
	AnimationControl(flag);

	SetAnimation();
	// アニメーションの更新
	float timeHandle = 0.0f;
	for (auto& model : models_) {
		timeHandle = model->GetAnimation()->ApplyAnimation(animationTime_);
	}
	animationTime_ = timeHandle;
	static const float frameSpeed = 1.0f / 60.0f;
	animationTime_ += frameSpeed;
	models_.at(static_cast<uint32_t>(Parts::Body))->GetAnimation()->Update(models_.at(static_cast<uint32_t>(Parts::Body))->GetTransform());
}

void PlayerAnimation::SetAnimation() {
	// アニメーションタイプに変化がなければ早期リターン
	if (oldType_ == nowType_) {
		return;
	}
	// しばらく補間かけて云々をここでやりたい気もする

	models_.at(static_cast<uint32_t>(Parts::Body))->SetAnimation(animation_.at(static_cast<uint32_t>(nowType_)).get());
}

void PlayerAnimation::SetAnimation(AnimationType type, const bool flag) {
	// 今のタイプが同じで、最初から再生するわけでもないのであれば、早期リターン
	if (nowType_ == type) {
		return;
	}
	oldType_ = nowType_;
	nowType_ = type;
	if (flag) {
		animationTime_ = 0.0f;
	}
}

void PlayerAnimation::AnimationControl(BehaviorFlag flag) {
	oldType_ = nowType_;
	if (flag.isReset) {
		animationTime_ = 0.0f;
	}
	// 下に配置すればするほど、優先度が高くなる
	if (flag.isWaiting) {
		nowType_ = AnimationType::Wait;
	}
	if (flag.isMoved) {
		nowType_ = AnimationType::Run;
	}
	if (flag.isJumped) {
		nowType_ = AnimationType::Jump;
	}

	// 今のタイプが待機中で、前まで待機していなかった場合
	if (CheckType(AnimationType::Wait)) {
		animationTime_ = 0.0f;
	} 
	else if (CheckType(AnimationType::Jump)) {
		animationTime_ = 0.0f;
	}

}

bool PlayerAnimation::CheckType(AnimationType type) const {
	// 今のタイプが変数で、前まで待機していなかった場合
	if ((nowType_ == type) && (nowType_ != oldType_)) {
		return true;
	}
	return false;
}
