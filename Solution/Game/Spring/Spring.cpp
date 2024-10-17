#include "Spring.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"

void Spring::Initialize() {
	anchor = Vector3(0.0f,1.0f,0.0f);
	naturalLength = 0.7f;
	stiffness = 100.0f;
	dampingCoefficient = 2.0f;

	ball.position = Vector3(0.8f, 0.2f, 0.0f);
	ball.mass = 2.0f;
	ball.radius = 0.05f;

	springLine_ = std::make_unique<Line>();
	ballModel_ = std::make_unique<Model>();
	ballModel_->SetModel(ResourceManager::GetInstance()->FindObject3d("Sphere"));


}

void Spring::Update() {
	ImGuiProcess();
	Move();

	static const Vector3 kGravity(0.0f, -9.8f, 0.0f);
	Vector3 diff = ball.position - anchor;
	float length = Length(diff);
	if (length != 0.0f) {
		Vector3 direction = diff.Normalize();
		//自然長に基づくウェブの理想位置を計算
		Vector3 restPosition = anchor + direction * naturalLength;
		// 変異を計算
		Vector3 displacement = (ball.position - restPosition);
		Vector3 restoringForce = displacement * -stiffness;
		Vector3 dampingForce = ball.velocity * -dampingCoefficient;
		Vector3 force = restoringForce + dampingForce + kGravity;
		ball.acceleration = force / ball.mass;
	}

	const float deltaTime = 1.0f / 60.0f;
	ball.velocity += ball.acceleration * deltaTime;
	ball.position += ball.velocity * deltaTime;

	// 今のベクトル
	Vector3 postVector = ball.position - anchor;
	float len = Length(postVector);
	if (len > naturalLength) {
		// 紐の長さまで戻してくる
		//ball.position = postVector.Normalize()* naturalLength;
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
	if (input->PressKey(DIK_A)) {
		ball.position.x -= kSpeed;
	}
	if (input->PressKey(DIK_D)) {
		ball.position.x += kSpeed;
	}
	if (input->PressKey(DIK_W)) {
		ball.position.z += kSpeed;
	}
	if (input->PressKey(DIK_S)) {
		ball.position.z -= kSpeed;
	}

	if (input->TriggerKey(DIK_SPACE)) {
		anchor += Vector3(1.0f, 0.0f, 0.0f);
	}

}
