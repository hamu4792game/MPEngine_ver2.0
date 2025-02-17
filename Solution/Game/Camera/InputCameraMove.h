#pragma once
#include "Framework/CameraFrame.h"

// カメラ移動用クラス
class inputCameraMove : public MPE::CameraFrame {
public:
	inputCameraMove();
	~inputCameraMove() = default;

	void Initialize(const WorldTransform& transform = WorldTransform()) override;

	void Update();

};
