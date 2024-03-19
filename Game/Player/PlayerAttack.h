#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class FollowCamera;

class PlayerAttack {
public:
	PlayerAttack() = default;
	void SetPtr(WorldTransform* playerTransPtr, WorldTransform* partsTransPtr, FollowCamera* followCameraPtr);
	~PlayerAttack() = default;

	void Initialize(); // 初期化処理
	bool Update(); // true:更新中 false:更新終了

private:
	void GetPhase(); // 今がどのフェーズ(振りかぶり状態なのか等)になるのか取得
	bool AttackProcess(); // 攻撃はじめの処理 false:終了
	void AttackAnimation(); // 攻撃のコンボ別アニメーション処理

private:

	// 攻撃用定数
	struct ConstAttack {
		uint32_t anticipationTime; // 振りかぶりの時間<frame>
		uint32_t chargeTime; // ため時間<frame>
		uint32_t swingTime; // 攻撃振りの時間
		uint32_t recoveryTime; // 硬直時間
		float anticipationSpeed; // 振りかぶりの移動速度
		float chargeSpeed; // ための移動速度
		float swingSpeed; // 攻撃振りの移動速度
	};
	// 攻撃用ワーク
	struct WorkAttack {
		// 攻撃ギミックの媒介変数
		uint32_t attackParameter_ = 0; // 今の攻撃の発生までの時間
		int32_t comboIndex_ = 0; // 今のコンボ
		int32_t inComboPhase_ = 0; // 今の振りかぶりやため、硬直時間のフェーズ
		bool comboNext_ = false; // コンボに派生させるか
	};
	WorkAttack workAttack_;
	uint32_t attackDamage_ = 0u; // 攻撃力
	static const int kComboNum = 3; // 最大コンボ定数
	// コンボ定数表
	static const std::array<ConstAttack, kComboNum> kConstAttacks_;

	// パーツ用WorldTransform
	WorldTransform* playerTrans = nullptr;
	WorldTransform* partsTrans = nullptr;

	// 追従カメラ
	FollowCamera* followCamera = nullptr;

};
