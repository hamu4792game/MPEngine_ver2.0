#pragma once
#include "Graphics/Particle/Particle.h"
#include <memory>

// プレイヤーのワイヤークラス
class Wire {
public:
	Wire();
	~Wire() = default;

	/// <summary>
	/// 始点と終点のセット その他初期化
	/// </summary>
	/// <param name="playerPos">playerのposition</param>
	/// <param name="anchor">アンカー ターゲットの座標</param>
	void Initialize(const Vector3& playerPos, const Vector3& anchor);

	void FadeOutUpdate();

	void FadeOutStart() {
		isFadeOut_ = true;
	}

private:
	void SetWireTipModel();

private:
	// ワイヤーの個々のモデル。particleとして扱っている
	std::unique_ptr<Particle> wireTipModels_;
	uint32_t wireTipNum_ = 0u;

	bool isFadeOut_ = false;

};