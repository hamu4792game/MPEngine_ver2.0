#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Utils/Collider/Collider.h"

class FollowCamera {
public:
	FollowCamera();
	~FollowCamera() = default;
	void Initialize(const WorldTransform& transform = WorldTransform());
	void SetParam(const Vector3& offset, const Vector3& rotate, float lerpSpeed = 0.2f);
	void Update(const float& speed);
	void DrawImGui();

	void LastUpdate();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const WorldTransform& GetTransform() const { return transform_; }
	const Vector3& GetPostposition() const { return postTranslate_; }
	void CameraMove();
	WorldTransform transform_;

	// 特定フレーム追従停止
	void StopFollow(const float& frame);

	bool OnCollision(const Collider& coll);

private:
	const WorldTransform* target_ = nullptr;
	Vector3 preTranslate_;
	Vector3 postTranslate_;
	Vector3 offset_;
	const Vector3 kMaxOffset_;
	Vector3 preOffset_;
	Vector3 preRotate_;
	Vector3 postRotate_;
	float lerpSpeed_ = 0.0f;

	bool isFollowStop_ = false;
	struct Timer {
		float nowFrame; // 今の時間
		float maxFrame; // 最大時間
	};
	Timer timer_;

	std::unique_ptr<Collider> collision_;
};
