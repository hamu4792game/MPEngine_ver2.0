#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Line/Line.h"


// スパイダーマンのウェブスイング
class WebSwing {
public:
	WebSwing();
	~WebSwing() = default;

	/// <summary>
	/// 値の初期化。Set
	/// </summary>
	/// <param name="anchor">アンカーポイント、ターゲットの座標</param>
	/// <param name="playerPos">Playerの座標</param>
	/// <param name="firstVel">初速度</param>
	void Initialize(const Vector3& anchor, const Vector3& playerPos, const Vector3& firstVel);

	void SetWeb(const Vector3& target, const Vector3& player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="player">playerの座標</param>
	/// <returns>返す移動ベクトル</returns>
	Vector3 Update(const Vector3& playerPos, bool& flag);


private: /// 構造体
	struct PlayerParam {
		Vector3 position; // playerの現在地
		Vector3 velocity; // ボールの速度
		Vector3 acceleration; // ボールの加速度
		float mass = 2.0f; // ボールの質量
		Vector3 moveVector; // キーで動かす移動ベクトル
	};

private:

	// 離した瞬間の処理
	Vector3 ReleaseWeb(PlayerParam& ball, Vector3 anchor, Vector3& swingDirection);

public:
	// 離された後の移動処理
	Vector3 ApplyAirMovement(PlayerParam& ball, float airDamping);


private:
	Vector3 target_;

	Vector3 anchor_; // アンカー、固定された橋の位置
	float naturalLength_ = 0.0f; // 紐の長さ
	float stiffness_ = 0.0f; // 剛性、ばね定数k
	float dampingCoefficient_ = 0.0f; // 減衰係数

	float hangTimeCounter = 0.0f;  // 滞空時間用のカウンター
	bool isHanging = false;  // 滞空状態のフラグ

	
	PlayerParam ball_;

	// swing中か
	bool isSwing_ = true;

	Vector3 moveVec_; // 移動ベクトル

	float maxLength_ = 0.0f; // 最大距離

	std::unique_ptr<Line> springLine_;
};
