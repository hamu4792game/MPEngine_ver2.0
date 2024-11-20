#pragma once
#include <memory>
#include "PlayerAnimation.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Camera/FollowCamera.h"
#include "WebSwing.h"
#include "WireTargetMove.h"
#include "WebSwing.h"
#include "Utils/Collider/Collider.h"

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager() = default;

	void Initialize(const WorldTransform& respawnpoint);
	void Update();
	WorldTransform PostUpdate(); // カメラ用更新処理 カメラ座標を返す

	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }
	// true ゲームクリア
	bool OnCollisionStage(const Collider& coll);
	WorldTransform OnCollisionCameraToStage(const Collider& coll);

	const uint32_t GetEffectNumber() const { return static_cast<uint32_t>(postEffectNum_); }
	Collider* GetCollision() const { return collision_.get(); }

private:
	const std::string itemName_ = "Player";
private:
	void DrawImGui(); // ImGui処理
	void InputMove(); // 移動処理
	void FalledProcess(); // 重力処理
	void TransformUpdate(); // 座標更新処理
	void LimitMoving(); // 移動制限用
	void KeyInput(); // プレイヤー入力処理まとめ

	void BehaviorRootUpdate();

	void SetGlobalVariables(); // データの値を外部ファイルからセット
	void AddGlobalVariables(); // データを外部ファイルに出力

private: // パラメータ構造体まとめ

	// key入力用一時変数まとめ
	struct InputParam {
		Vector3 move; // 入力方向ベクトル
		bool isJump = false; // ジャンプボタンが押されたか
		bool isWireMove = false; // ワイヤー移動ボタンが押されたか
		bool isDashMove = false; // ダッシュボタンが押されたか
		bool isSwingMove = false; // ウェブスイングをするか
		bool isPushSwing = false; // ウェブスイングを押した瞬間

		void Initialize() {
			move = Vector3::zero;
			isJump = false;
			isWireMove = false;
			isDashMove = false;
			isSwingMove = false;
			isPushSwing = false;
		}
	};

	// 速度パラメーター
	struct SpeedParam {
		float acceleration = 0.0f; // 加速度
		float accelerationRate = 0.1f; // 加速率
		float kMinAcceleration = 0.0f; // 最低加速度
		float kMaxAcceleration = 5.0f; // 最大加速度
		void Initialize(const float& acc, const float& rate, const float& min, const float& max) {
			acceleration = acc; // 初速度を与えてる
			accelerationRate = rate;
			kMinAcceleration = min;
			kMaxAcceleration = max;
		}
		void AccelInit() {
			acceleration = 0.0f;
		}
		void AddUpdate(); // 加算更新
	};

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		SpeedParam fall; // 落下時の加速度等
		bool isFalled = false; // 落下中かのフラグ true:落ちている/false:落ちていない
		bool isJump = false; // ジャンプするかのフラグ。キー入力時に立つ
		bool isJumpable = true; // ジャンプ可能かのフラグ
		bool oldJumpable = false; // 前の状態フラグ
		// ジャンプ用の初期化処理
		void JumpInitialize() {
			isFalled = false;
			isJumpable = true;
			oldJumpable = false;
			fall.acceleration = 0.0f;
		}
	};

	// webswing用ステータス
	struct WebSwingParam {
		Vector3 position; // playerの現在地
		Vector3 velocity; // ボールの速度
		Vector3 acceleration; // ボールの加速度
		float mass = 2.0f; // ボールの質量
		Vector3 moveVector; // キーで動かす移動ベクトル
	};
	

private:
	std::unique_ptr<PlayerAnimation> animation_;
	PlayerAnimation::BehaviorFlag behaviorFlag_;
	WorldTransform transform_;
	Vector3 moveVector_; // 移動ベクトル

	Vector3 velocity_; // 現在の速度
	Vector3 oldPosition_; // 1つ前のプレイヤーワールド座標
	

	
	InputParam inputParam_;

	

	// 移動関係速度
	SpeedParam moveParameter_;

	Vector3 oldMoveVector; // 1f前の移動ベクトル

	

	FallParam fallParam_;

	// 振る舞い
	enum class Behavior {
		kRoot,	// 通常行動
		kWebSwing,	// ウェブスイング
		kDash,	// ダッシュ中
		kWireMove,	// ワイヤー移動中
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	const WorldTransform* targetTransform_ = nullptr;

	std::unique_ptr<Collider> collision_;
	WorldTransform collTrans_;

	std::unique_ptr<WebSwing> webswing_;
	std::unique_ptr<WireTargetMove> wireTargetMove_;

	// カメラ関係
	std::shared_ptr<FollowCamera> followCamera_;

	WorldTransform respawnpoint_;

	enum PostEffectNum {
		None,
		RadialBlur,
		GrayScale,
	};
	PostEffectNum postEffectNum_;

	float masterSpeed_ = 1.0f;
	struct FrameCount {
		float maxFrame = 1.0f;
		float count = 0.0f;
	};
	FrameCount frameCount_;

	Vector3 hittingObjectNormal_; // 衝突している物体の法線ベクトル


	// ウェブスイング用
	bool isWebSwing_ = false;

};
