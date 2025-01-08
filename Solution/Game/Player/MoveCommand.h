#pragma once
#include <memory>
#include <string>
#include "Game/Global/AllParameter.h"
#include "WireTargetMove.h"
#include "WebSwing.h"

class MoveCommand {
public:
	MoveCommand() = default;
	~MoveCommand() = default;
	MoveCommand(const std::string& itemName);

	// Ex = 実行処理、初期化と同義
	// Up = 更新処理

	// Playerの移動に必要なパラメーターを取得
	void Initialize(MoveParam param, const float* masterSpeed);

	/// <summary>
	/// 通常入力の移動処理
	/// </summary>
	/// <param name="inputMove">入力された移動ベクトル</param>
	
	/// <summary>
	/// 入力時の移動
	/// </summary>
	/// <param name="inputMove">入力された移動ベクトル</param>
	/// <param name="moveVolume">移動量やその他のパラメーターを参照渡しする</param>
	/// <param name="isLanded">true:地面についている/false:滞空状態</param>
	/// <returns>true:移動している/false:移動していない</returns>
	bool UpInputMove(Vector3 inputMove, WorldTransform& moveVolume, const bool& isLanded);

	// 実行するための初期化用
	void ExWireTargetMove(const Vector3& target, const Vector3& player);
	// ターゲットまでワイヤー移動を行う更新
	bool UpWireTargetMove(const Vector3& player, Vector3& result);

	// ジャンプ処理
	void ExJump(FallParam& param, const Vector3& direction = Vector3::zero);

	// 落下処理
	Vector3 UpFalling(FallParam& param);

	// ウェブスイングの初期化用
	void ExWebSwing(const WorldTransform& player, const float& firstVel);
	// ウェブスイングの更新処理
	Vector3 UpWebSwing(const Vector3& playerPos, bool& isWebSwing);

	// ウェブスイングの事後処理、離された後の座標移動
	Vector3 UpPostWebSwing();

	/// <summary>
	/// 壁移動の更新処理
	/// </summary>
	/// <param name="hitNormal">壁に当たっている正規化ベクトル</param>
	/// <param name="playerRotate">Playerの回転Quaternionの参照渡し用変数</param>
	/// <param name="moveVec">入力移動ベクトル</param>
	/// <returns>移動ベクトルを返す</returns>
	Vector3 UpWallMove(const Vector3& hitNormal, Quaternion& playerRotate, const Vector3& moveVec);

	Vector3 UpSkyDash();

public: // ゲッター
	const MoveParam& GetParameter() const { return param_; }

	void ImGuiProc(); // imGui用関数
private: // ローカル関数

private:
	const std::string itemName_;
	// 入れ子 基本const扱いする
	MoveParam param_;
	std::unique_ptr<WireTargetMove> wireTargetMove_;
	std::unique_ptr<WebSwing> webSwing_;
	const float* masterSpeed_ptr = nullptr;

	Vector3 webswingDirection_;
	Vector3 jumpDirection_; // ジャンプする際の方向ベクトル。現在はY軸が無関係で、速度も入ってるから実質Velocity

};
