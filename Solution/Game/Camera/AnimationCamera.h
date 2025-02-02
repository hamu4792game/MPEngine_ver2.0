#pragma once
//#include "MPEngine/Utils/Camera/Camera.h"
#include <string>
#include "Math/MathUtl.h"

class AnimationCamera {
public:
	AnimationCamera() = default;
	~AnimationCamera() = default;

	void Initialize();

private:
	void SetGlobalVariables(); // データの値を外部ファイルからセット
	void AddGlobalVariables(); // データを外部ファイルに出力

private:
	const std::string animationName = "A";
	struct KeyParam {
		std::string key; // キー番号
		float frame = 0.0f; // 何フレームで次のキーに行くか
		Vector3 rotate; // 回転

	};
	uint32_t keyCount_ = 0u; // キーの数


};