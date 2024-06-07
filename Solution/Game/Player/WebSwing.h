#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

// スパイダーマンのウェブスイング
class WebSwing {
public:
	WebSwing();
	~WebSwing() = default;

	void SetWeb(const Vector3& target, const Vector3& player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="player">playerの座標</param>
	/// <param name="result">引数に返す移動ベクトル</param>
	/// <returns>処理終了:true/処理中:false</returns>
	bool Update(const Vector3& player, Vector3& result);
private:

	Vector3 target_;

	Vector3 moveVec_; // 移動ベクトル

	float maxLength_ = 0.0f; // 最大距離
};
