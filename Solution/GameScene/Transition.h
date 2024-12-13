#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <memory>

// シーントランジション用のクラス
class Transition {
public:
	Transition();
	~Transition() = default;


public:
	enum class Type {
		Spin = 0,
		BlackOut,

		kMaxNum
	};
public:

	// 更新処理
	bool Update();

	// タイプを指定した初期化のようなもの
	void StartTransition(Type type);

private:
	// 終了時の追加処理
	void EndProcess();

	bool ScalingSpinChange(); // 拡縮回転切り替え

	bool FadingInOut(); // 徐々に暗くなったり明るくなったりする切り替え

private:
	std::unique_ptr<Sprite> background_;

	bool isTransition_ = false; // トランジションがされているか true:してる / false:していない
	bool fadeInOutFlag_ = true; // true:In / false:Out
	Type type_ = Type::Spin;

	Vector2 scale_;
	const float fadeSpeed_;
	float rotate_ = 0.0f;
	Vector4 color_;


};
