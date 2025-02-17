#pragma once
#include "Framework/CameraFrame.h"
#include "Utils/Collider/Collider.h"

// 追従カメラクラス
class FollowCamera : public MPE::CameraFrame {
public:
	FollowCamera();
	~FollowCamera() = default;

	void Initialize(const WorldTransform& transform = WorldTransform()) override;

	void Update();

};
