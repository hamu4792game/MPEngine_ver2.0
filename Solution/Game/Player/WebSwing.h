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
	/// <param name="result">引数に返す移動ベクトル</param>
	/// <returns>処理終了:true/処理中:false</returns>
	bool Update(const Vector3& player, Vector3& result);

	Vector3 Update(const Vector3& playerPos);

private:
	Vector3 target_;

	Vector3 anchor_; // アンカー、固定された橋の位置
	float naturalLength_ = 0.0f; // 紐の長さ
	float stiffness_ = 0.0f; // 剛性、ばね定数k
	float dampingCoefficient_ = 0.0f; // 減衰係数

	struct PlayerParam {
		Vector3 position; // playerの現在地
		Vector3 velocity; // ボールの速度
		Vector3 acceleration; // ボールの加速度
		float mass = 2.0f; // ボールの質量
		Vector3 moveVector; // キーで動かす移動ベクトル
	};
	PlayerParam ball_;

	// swing中か
	bool isSwing_ = true;

	Vector3 moveVec_; // 移動ベクトル

	float maxLength_ = 0.0f; // 最大距離

	std::unique_ptr<Line> springLine_;
};
