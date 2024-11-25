#pragma once
#include <memory>
#include <string>
#include "WireTargetMove.h"
#include "Game/Global/AllParameter.h"

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
	/// <param name="moveVolume">移動量やその他のパラメーターを参照渡しする</param>
	/// <returns>true:移動している/false:移動していない</returns>
	bool UpInputNormalMove(Vector3 inputMove, WorldTransform& moveVolume);

	// 実行するための初期化用
	void ExWireTargetMove(const Vector3& target, const Vector3& player);
	// ターゲットまでワイヤー移動を行う更新
	bool UpWireTargetMove(const Vector3& player, Vector3& result);

	// ジャンプ処理
	bool ExJump(FallParam& param);

	// 落下処理
	Vector3 UpFalling(FallParam& param);

public: // ゲッター
	const MoveParam& GetParameter() const { return param_; }

	void ImGuiProc(); // imGui用関数
private: // ローカル関数

private:
	const std::string itemName_;
	MoveParam param_;
	std::unique_ptr<WireTargetMove> wireTargetMove_;
	const float* masterSpeed_ptr = nullptr;
};
