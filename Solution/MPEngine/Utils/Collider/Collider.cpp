#include "Collider.h"
#include "Math/AABB.h"

void Collider::Initialize(const WorldTransform& transform, Type type) {
	collderType_ = type;
	transform_ = transform;
	switch (collderType_) {
	case Collider::Box:
		boxCollider_ = std::make_shared<AABB>();
		break;
	case Collider::Sphere:
		break;
	}
}

void Collider::Update() {
	switch (collderType_) {
	case Collider::Box:
		boxCollider_->Update(transform_);
		break;
	case Collider::Sphere:
		break;
	}
}

bool Collider::OnCollision(const Collider& coll, Vector3& pushbackVec) {

	switch (coll.collderType_) {
	case Collider::Box:
		BoxCollision(coll, pushbackVec);
		break;
	case Collider::Sphere:
		break;
	}
	return false;
}

bool Collider::BoxCollision(const Collider& coll, Vector3& pushbackVec) {

	switch (collderType_) {
	case Collider::Box:
		return boxCollider_->IsCollision(coll.boxCollider_.get());
	case Collider::Sphere:
		return false;
	}
	return false;
}
