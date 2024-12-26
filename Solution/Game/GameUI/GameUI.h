#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <vector>
#include <array>
#include <string>

class GameUI {
public:
	GameUI() = default;
	~GameUI() = default;

	void Initialize(const uint32_t& collectionNum);
	void Update(const uint32_t& collectionNum);

	// 最初に取得した場合の更新 終了したらtrueを返す
	bool FirstUpdate(const uint32_t& collectionNum);

private:
	// 収集物が取得されている状態かの更新
	void CollectionObjectActiveUpdate(const uint32_t& collectionNum);

	void ImGuiProc(); // debug用
	void SetGlobalVariables(); // データの値を外部ファイルからセット
	void AddGlobalVariables(); // データを外部ファイルに出力

private:
	const std::string itemName_ = "GameUI";

	// 唯一性のあるHud
	enum UniqueHud {
		MoveKey,
		Mouse,
		Escape,

		kMaxNum
	};
	// hudのtextureの名前
	const std::array<std::string, UniqueHud::kMaxNum> kTextureNames_{
		"MoveKey",
		"Mouse",
		"Escape"
	};
	// hud用パラメーター構造体
	struct HudParameter {
		Vector2 scale;
		Vector2 translate;
		Vector2 uvScale;
		Vector2 uvTranslate;
		HudParameter() = default;
		HudParameter(Vector2 s, Vector2 t, Vector2 us, Vector2 ut) : scale(s), translate(t), uvScale(us), uvTranslate(ut) {}
	};

	// 収集物用パラメーター構造体
	struct CollectionParam {
		HudParameter hudParam;
		float distance = 0.0f; // スプライトの間隔
	};

	// hudのsprite
	std::array<std::shared_ptr<Sprite>, UniqueHud::kMaxNum> huds_;
	// 収集物用sprite
	std::vector<std::shared_ptr<Sprite>> collectionObjects_;
	// Hud用のパラメーター
	std::array<HudParameter, UniqueHud::kMaxNum> hudParams_;
	// 収集物用のパラメーター 最初の一つだけ必要
	CollectionParam collectionObjectsParameter_;
	// 収集物の前フレーム情報を取得する変数
	uint32_t oldCollectionobjectNum_ = 0u;
	
	std::shared_ptr<Sprite> backGround_; // 背景用
	
	// チュートリアルアニメーション？用のフレーム
	float nowFrame_ = 0.0f;
	// easeInOut用フラグ false = 拡大/true = 縮小
	bool isScale_ = false;


};
