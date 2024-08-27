#include "Collider.h"
#include "Graphics/Line/Line.h"

void Collider::Initialize(WorldTransform* transform, Type type, std::string name) {
	collderType_ = type;
	transform_ = transform;
	name_ = name;
	switch (collderType_) {
	case Collider::Type::Box:
		if (!boxCollider_) {
			boxCollider_ = std::make_unique<BoxCollider>();
		}
		lines_.resize(12);
		break;
	case Collider::Type::Sphere:
		break;
	case Collider::Type::Line:
		if (!lineCollider_) {
			lineCollider_ = std::make_unique<LineCollider>();
		}
		lines_.resize(1);
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
	case Collider::Type::Box:
		boxCollider_->Update(*transform_); 
		boxCollider_->LineUpdate(lines_);
		break;
	case Collider::Type::Sphere:
		break;
	case Collider::Type::Line:
		lineCollider_->Update(*transform_);
		lineCollider_->LineUpdate(lines_);
		break;
	}
	
}

void Collider::SetLineColor(Vector4 color) {
	for (auto& i : lines_) {
		i->SetColor(color);
	}
}

bool Collider::OnCollision(const Collider& coll, Vector3& pushbackVec) {
	bool flag = false;
	switch (coll.collderType_) {
	case Collider::Type::Box:
		flag = BoxCollision(coll, pushbackVec);
		break;
	case Collider::Type::Sphere:
		break;
	}
	return flag;
}

bool Collider::BoxCollision(const Collider& coll, Vector3& pushbackVec) {
	pushbackVec = Vector3::zero;
	Vector3 minAxis;
	float minOverlap = 0.0f;
	switch (collderType_) {
	case Collider::Type::Box:
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
	case Collider::Type::Sphere:
		return false;
		break;
	case Collider::Type::Line:
		if (coll.boxCollider_->IsCollision(*lineCollider_, minAxis, minOverlap)) {
			pushbackVec = minAxis;
			return true;
		}
		break;
	}
	return false;
}
