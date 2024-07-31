#include "LineCollider.h"
#include "MPEngine/Graphics/Line/Line.h"

void LineCollider::Update(const WorldTransform& transform) {
	ray_.origin = transform.GetPosition();

	Vector3 cameraDirection = MakeRotateMatrix(transform.rotation_) * Vector3::down;
	ray_.diff = cameraDirection;
}

void LineCollider::LineUpdate(std::vector<std::shared_ptr<Line>> lines) {
	Vector3 vertices[2]{
		ray_.origin,
		ray_.origin + (ray_.diff * 10.0f),
	};
	lines[0]->SetLine(vertices[0], vertices[1]);
}
