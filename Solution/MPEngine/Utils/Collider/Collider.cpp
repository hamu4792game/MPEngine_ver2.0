#include "Collider.h"
#include "Graphics/Line/Line.h"

void Collider::Initialize(WorldTransform* transform, Type type) {
	collderType_ = type;
	transform_ = transform;
	switch (collderType_) {
	case Collider::Box:
		if (!boxCollider_) {
			boxCollider_ = std::make_unique<BoxCollider>();
		}
		lines_.resize(12);
		break;
	case Collider::Sphere:
		break;
	}
	for (auto& i : lines_) {
		if (!i) {
			i = std::make_shared<Line>();
		}
#ifndef _DEBUG
		// debugだった場合。表示を削除
		i->isActive_ = false;
#endif // _DEBUG
	}
}

void Collider::Update() {
	transform_->UpdateMatrix();
	switch (collderType_) {
	case Collider::Box:
		boxCollider_->Update(*transform_); 
		boxCollider_->LineUpdate(lines_);
		break;
	case Collider::Sphere:
		break;
	}
	
}

bool Collider::OnCollision(const Collider& coll, Vector3& pushbackVec) {
	bool flag = false;
	switch (coll.collderType_) {
	case Collider::Box:
		flag = BoxCollision(coll, pushbackVec);
		break;
	case Collider::Sphere:
		break;
	}
	return flag;
}

bool Collider::BoxCollision(const Collider& coll, Vector3& pushbackVec) {
	pushbackVec = Vector3::zero;
	Vector3 minAxis;
	float minOverlap = 0.0f;
	switch (collderType_) {
	case Collider::Box:
		if (boxCollider_->IsCollision(*coll.boxCollider_.get(), minAxis, minOverlap)) {
			// 当たってたら
			float dot = Dot(coll.transform_->GetPosition() - this->transform_->GetPosition(), minAxis);
			if (dot > 0.0f) {
				minOverlap = -minOverlap;
			}
			pushbackVec = Normalize(minAxis) * minOverlap;
			return true;
		}
		break;
	case Collider::Sphere:
		return false;
		break;
	}
	return false;
}
