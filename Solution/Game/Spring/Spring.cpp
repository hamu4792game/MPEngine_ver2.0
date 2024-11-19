#include "Spring.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"

void Spring::Initialize() {
	anchor = Vector3(0.0f,0.0f,10.0f);
	naturalLength = 0.7f;
	stiffness = 100.0f;
	dampingCoefficient = 2.0f;

	ball.position = Vector3(1.0f, -1.0f, 10.0f);
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
		ApplyAirMovement(ball, 0.98f);
		//ball.position += ball.velocity * deltaTime;
	}
	else {
		const float speedThreshold = 0.02f;  // 滞空の閾値速度
		const float hangTimeDuration = 10.0f / 60.0f;  // 滞空時間
		

		Vector3 diff = ball.position - anchor;
		float length = Length(diff);

		if (length > 0.0f) {
			Vector3 direction = diff.Normalize();

			// 滞空状態の処理
			if (isHanging) {
				hangTimeCounter += deltaTime;
				if (hangTimeCounter >= hangTimeDuration) {
					isHanging = false;         // 滞空時間が終了したら滞空状態を解除
					hangTimeCounter = 0.0f;    // カウンターをリセット
					isSwing_ = false;
					ReleaseWeb(ball, anchor, direction);
				}
				return; // 滞空中は物理演算を行わない
			}

			// ウェブの張力（アンカーに引っ張られる力）
			Vector3 restoringForce = direction * (-stiffness * (length - naturalLength));

			// ダンピングの計算（減衰力）を強めに設定
			Vector3 dampingForce = ball.velocity * -dampingCoefficient * 1.5f;

			// 合力の計算
			Vector3 force = restoringForce + kGravity + ball.moveVector;

			// 加速度と速度を更新
			ball.acceleration = force / ball.mass;
			ball.velocity += ball.acceleration * deltaTime;

			// 張力方向に沿った速度成分を除去して、振り子運動を維持
			Vector3 velocityAlongRope;
			float dotProduct = ball.velocity.x * direction.x + ball.velocity.y * direction.y + ball.velocity.z * direction.z;
			float lengthSq = direction.Length();
			lengthSq = lengthSq * lengthSq;
			if (lengthSq > 0.0f) {
				velocityAlongRope = direction * (dotProduct / lengthSq);
			}
			ball.velocity = ball.velocity - velocityAlongRope;

			// ボールの位置を更新
			ball.position += ball.velocity * deltaTime;

			// 常に自然長を保つ
			Vector3 toAnchor = ball.position - anchor;
			float newLength = toAnchor.Length();

			// スイングのアーチ状の動きを保持するために、ウェブの長さを自然長に制約
			if (newLength > naturalLength) {
				ball.position = anchor + toAnchor.Normalize() * naturalLength;
			}

			// 速度が閾値以下なら滞空状態に入る
			// 多分後で消えて、離された時に力をそのまま渡すみたいな感じになりそう
			float leg = ball.velocity.Length();
			if (leg < speedThreshold) {
				//isHanging = true;
			}

			// 角度が90°以上なら滞空状態に入る
			float angle = FindAngle(Vector3::down, direction);
			if (std::fabsf(angle) >= AngleToRadian(90.0f)) {
				isHanging = true;
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
	ImGui::DragFloat("ball.mass", &ball.mass, 0.1f, 0.0f, 10.0f);
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

		if (anchor.x >= ball.position.x) {
			anchor -= Vector3(1.0f, 0.0f, 0.0f);
		}
		else {
			anchor += Vector3(1.0f, 0.0f, 0.0f);
		}

		float len = (ball.position - anchor).Length();
		naturalLength = len;
	}
	if (input->TriggerKey(DIK_R)) {
		isSwing_ = true;

		anchor = Vector3(0.0f, 0.0f, 10.0f);
		naturalLength = 0.7f;
		stiffness = 100.0f;
		dampingCoefficient = 2.0f;

		ball.position = Vector3(1.0f, -1.0f, 10.0f);
		ball.velocity = Vector3::zero;
		ball.mass = 2.0f;
	}

}

Vector3 Spring::ReleaseWeb(Ball& ball, Vector3 anchor, Vector3& swingDirection) {
	const float jumpStrength = 0.1f;  // ジャンプ初速の強さ
	const float airDamping = 0.98f;    // 空中での減衰効果

	// スイング方向に基づいて初速を設定
	Vector3 jumpVelocity = (swingDirection + Vector3::up).Normalize() * jumpStrength;
	ball.velocity = jumpVelocity;

	// 空中挙動（ジャンプ中の更新処理）
	return ApplyAirMovement(ball, airDamping);
}

Vector3 Spring::ApplyAirMovement(Ball& ball, float airDamping) {
	const float deltaTime = 0.1f;
	static const Vector3 kGravity(0.0f, -0.00098f, 0.0f);

	// 空中での減衰と重力の影響
	ball.velocity *= airDamping;
	ball.velocity += kGravity;
	ball.position += ball.velocity;

	return ball.velocity;
}
