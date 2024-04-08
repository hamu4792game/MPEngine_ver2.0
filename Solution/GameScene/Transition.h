#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <memory>

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

	bool Update();

	void StartTransition(Type type);

private:
	void EndProcess();

	bool ScalingSpinChange(); // 拡縮回転切り替え

	bool FadingInOut(); // 徐々に暗くなったり明るくなったりする切り替え

private:
	std::unique_ptr<Sprite> background_;

	bool isTransition_ = false; // トランジションがされているか true:してる / false:していない
	bool fadeInOutFlag_ = true; // true:In / false:Out
	Type type_;

	Vector2 scale_;
	const float fadeSpeed_;
	float rotate_;
	Vector4 color_;


};
