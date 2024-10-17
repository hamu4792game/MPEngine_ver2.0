#include "Spring.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"

void Spring::Initialize() {
	anchor = Vector3(0.0f,1.0f,0.0f);
	naturalLength = 0.7f;
	stiffness = 100.0f;
	dampingCoefficient = 2.0f;

	ball.position = Vector3(0.0f, 0.7f, 0.8f);
	ball.mass = 2.0f;
	ball.radius = 0.05f;

	springLine_ = std::make_unique<Line>();
	ballModel_ = std::make_unique<Model>();
	ballModel_->SetModel(ResourceManager::GetInstance()->FindObject3d("Sphere"));


}

void Spring::Update() {
	ImGuiProcess();
	Move();
	const float deltaTime = 1.0f / 60.0f;
	static const Vector3 kGravity(0.0f, -9.8f, 0.0f);

	if (!isSwing_) {
		//ball.velocity += (ball.acceleration + kGravity) * deltaTime;
		ball.position += ball.velocity * deltaTime;
	}
	else {
		Vector3 diff = ball.position - anchor;
		float length = Length(diff);
		if (length > 0.0f) {
			Vector3 direction = diff.Normalize();
			// ウェブの張力（アンカーに引っ張られる力）
			Vector3 restoringForce = direction * (-stiffness * (length - naturalLength));
			// ダンピングの計算（減衰力）
			Vector3 dampingForce = ball.velocity * -dampingCoefficient;
			// 合力の計算
			Vector3 force = restoringForce + kGravity + ball.moveVector;

			// 速度と位置の更新
			ball.acceleration = force / ball.mass;
			ball.velocity += (ball.acceleration) * deltaTime;

			// 張力方向に沿った痩躯度成分を除去して振り子運動を維持
			Vector3 velocityAlongRope;
			float dotProduct = ball.velocity.x * direction.x + ball.velocity.y * direction.y + ball.velocity.z * direction.z;
			float lengthSq = direction.Length();
			lengthSq = lengthSq * lengthSq;
			if (lengthSq > 0.0f) {
				velocityAlongRope = direction * (dotProduct / lengthSq);
			}
			ball.velocity = ball.velocity - velocityAlongRope;

			ball.position += ball.velocity * deltaTime;

			// 常に自然長を保つ
			Vector3 toAnchor = ball.position - anchor;
			float newLength = toAnchor.Length();

			// スイングのアーチ状の動きを保持するために、ウェブの長さを自然長に制約
			if (newLength > naturalLength) {
				// 長さを調整し、スイングの円運動を保つ
				ball.position = anchor + toAnchor.Normalize() * naturalLength;
			}
		}
	}

	springLine_->SetLine(anchor, ball.position);
	ballModel_->SetTransform(WorldTransform(Vector3(ball.radius, ball.radius, ball.radius), Vector3::zero, ball.position));

}

void Spring::ImGuiProcess() {
#ifdef _DEBUG
	ImGui::Begin("Ball");
	ImGui::DragFloat3("anchor", &anchor.x, 0.1f);
	ImGui::DragFloat("naturalLength", &naturalLength, 0.1f);
	ImGui::DragFloat("stiffness", &stiffness, 0.1f);
	ImGui::DragFloat("dampingCoefficient", &dampingCoefficient, 0.1f);
	ImGui::DragFloat3("ball.position", &ball.position.x, 0.1f);
	ImGui::DragFloat3("ball.velocity", &ball.velocity.x, 0.1f);
	ImGui::DragFloat3("ball.acceleration", &ball.acceleration.x, 0.1f);
	ImGui::DragFloat("ball.mass", &ball.mass, 0.1f);
	ImGui::DragFloat("ball.radius", &ball.radius, 0.1f);
	ImGui::End();

#endif // _DEBUG
}

void Spring::Move() {
	auto input = Input::GetInstance()->GetKey();
	const float kSpeed = 0.5f;
	ball.moveVector = Vector3::zero;
	if (input->PressKey(DIK_A)) {
		ball.moveVector.x -= kSpeed;
	}
	if (input->PressKey(DIK_D)) {
		ball.moveVector.x += kSpeed;
	}
	if (input->PressKey(DIK_W)) {
		ball.moveVector.z += kSpeed;
	}
	if (input->PressKey(DIK_S)) {
		ball.moveVector.z -= kSpeed;
	}

	if (input->TriggerKey(DIK_SPACE)) {
		anchor += Vector3(1.0f, 0.0f, 0.0f);
		float len = (ball.position - anchor).Length();
		naturalLength = len;
	}
	if (input->TriggerKey(DIK_P)) {
		isSwing_ = !isSwing_;
	}

}
