#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Animation/ModelAnimation.h"
#include <array>
#include <memory>
#include <string>

class PlayerAnimation {
public:
	PlayerAnimation() = default;
	PlayerAnimation(const WorldTransform* transform);
	~PlayerAnimation() = default;

private:
	const std::string itemName_ = "Player";
public: // public変数
	enum class Parts {
		Body,
		kMaxNum
	};
	enum class AnimationType {
		Wait,
		Run,
		Jump,
		kMaxNum
	};

	struct BehaviorFlag {
		bool isWaiting = false; // 待機中か
		bool isMoved = false; // 移動中か
		bool isJumped = false; // ジャンプ中か
		bool isFalled = false; // 落下中か
		bool isLanded = false; // 着地したか
		bool isReset = false; // animationTimeをリセットするか
		bool isLoop = false;
		void Initialize() {
			isWaiting = false;
			isMoved = false;
			isJumped = false;
			isFalled = false;
			isLanded = false;
			isReset = false;
		}
	};
public: // public関数
	void Initialize();
	void Update(BehaviorFlag flag);

	// アニメーションタイプのセット、flag = true:最初から再生/false:途中から再生
	void SetAnimation(AnimationType type, const bool flag = false);

	const bool& GetFinishedAnimation() const { return isFinishedAnimation_; }

	void SetQuaternion(const Quaternion& qua);
	void SetColor(const Vector4& color) { models_.front()->SetColor(color); }

private: // private関数
	float AnimationUpdate(BehaviorFlag flag); // アニメーションの更新
	void SetAnimation(); // アニメーション切り替えの更新
	void AnimationControl(BehaviorFlag flag);
	bool CheckType(AnimationType type) const;

private: // private変数
	std::array<std::unique_ptr<Model>, static_cast<uint32_t>(Parts::kMaxNum)> models_;
	std::array<std::unique_ptr<ModelAnimation>, static_cast<uint32_t>(AnimationType::kMaxNum)> animation_;
	float animationTime_ = 0.0f;
	AnimationType nowType_; // 今のタイプ
	AnimationType oldType_; // 前のタイプ
	bool isFinishedAnimation_ = false; // アニメーションが終わったか
	bool isJumpFrag_ = false;
};
