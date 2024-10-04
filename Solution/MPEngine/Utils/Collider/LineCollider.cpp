#include "LineCollider.h"
#include "MPEngine/Graphics/Line/Line.h"

void LineCollider::Update(const WorldTransform& transform, const WorldTransform& targetTrans) {
	ray_.origin = transform.GetPosition();
	ray_.end = targetTrans.GetPosition();

	ray_.diff = (ray_.end - ray_.origin).Normalize();
}

void LineCollider::LineUpdate(std::vector<std::shared_ptr<Line>> lines) {
	Vector3 vertices[2]{
		ray_.origin,
		ray_.end,
	};
	lines[0]->SetLine(vertices[0], vertices[1]);
}
