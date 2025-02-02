#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Utils/Collider/Collider.h"

// 追従カメラクラス
class FollowCamera {
public:
	FollowCamera();
	~FollowCamera() = default;
	void Initialize(const WorldTransform& transform = WorldTransform());
	void SetParam(const Vector3& offset, const Vector3& rotate, float lerpSpeed = 0.2f);
	void Update(const float& speed);
	void DrawImGui();

	void LastUpdate();

	// カメラを手動で移動する
	bool CameraMove();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetCameraStateRotate(const Quaternion& qur) { cameraStateRotate_ = qur; }
	const WorldTransform& GetTransform() const { return transform_; }
	const Vector3& GetPostposition() const { return postTranslate_; }
	const bool& GetIsCameraMove() const { return isCameraMove_; }
	WorldTransform transform_;

	// 特定フレーム追従停止
	void StopFollow(const float& frame);

	bool OnCollision(const Collider& coll);

private:
	const WorldTransform* target_ = nullptr;
	Vector3 postTranslate_;
	Vector3 offset_;
	const Vector3 kMaxOffset_;
	Vector3 preOffset_;
	Vector3 preRotate_;
	Vector3 postRotate_;
	Quaternion moveRotateQuaternion_;
	float lerpSpeed_ = 0.0f;

	Quaternion rotate_;

	bool isFollowStop_ = false;
	struct Timer {
		float nowFrame = 0.0f; // 今の時間
		float maxFrame = 0.0f; // 最大時間
		void SetMaxFrame(const float& max) { maxFrame = max; }
		void TimerReset() { nowFrame = 0.0f; }
	};
	Timer timer_;

	std::unique_ptr<Collider> collision_;

	bool isCameraMove_ = false; // カメラが動いていなかったらのフラグ
	bool isOldCameraMove_ = false; // カメラが動いていなかったらのフラグ
	Quaternion cameraStateRotate_;
	Vector2 oldCameraRotateMove_;

};
