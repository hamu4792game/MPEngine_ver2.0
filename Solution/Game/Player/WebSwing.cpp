#include "WebSwing.h"
#include "ImGuiManager/ImGuiManager.h"

WebSwing::WebSwing() {
	springLine_ = std::make_unique<Line>();
	springLine_->SetColor(0xff0000ff);
}

void WebSwing::Initialize(const Vector3& anchor, const Vector3& playerPos, const Vector3& firstVel) {
    anchor_ = anchor;
    naturalLength_ = (playerPos - anchor).Length(); // 紐の長さを設定
    ball_.velocity = firstVel; // 初速度の設定
	ball_.mass = 50.0f;

	stiffness_ = 100.0f;
	dampingCoefficient_ = 2.0f;
	isSwing_ = true;
}

void WebSwing::SetWeb(const Vector3& target, const Vector3& player) {
    target_ = target;
    Vector3 vec = FindVector(player, target_);
	float distance = Length(vec);
    moveVec_ = vec.Normalize();
    maxLength_ = Length(vec); // ロープの長さ。最初にセットしたタイミングで決まる
}

Vector3 WebSwing::Update(const Vector3& playerPos, bool& flag) {
	const float deltaTime = 1.0f;
	static const Vector3 kGravity(0.0f, -9.8f, 0.0f);
	
	if (!isSwing_) {
		// 現状の仕様では通っていない
		//moveVec_ = ball_.velocity * deltaTime;
		moveVec_ = ApplyAirMovement(ball_, 0.0098f);
	}
	else {
		Vector3 diff = playerPos - anchor_;
		float length = Length(diff);
		if (length > 0.0f) {
			Vector3 direction = diff.Normalize();

			const float speedThreshold = 0.02f;  // 滞空の閾値速度
			const float hangTimeDuration = 10.0f / 60.0f;  // 滞空時間
			// 滞空状態の処理
			if (isHanging) {
				hangTimeCounter += speedThreshold;
				if (hangTimeCounter >= hangTimeDuration) {
					isHanging = false;         // 滞空時間が終了したら滞空状態を解除
					hangTimeCounter = 0.0f;    // カウンターをリセット
					isSwing_ = false;
					ReleaseWeb(ball_, anchor_, direction);
				}
				return ball_.velocity; // 滞空中は物理演算を行わない
			}

			// ウェブの張力（アンカーに引っ張られる力）
 			Vector3 restoringForce = direction * (-stiffness_ * (length - naturalLength_));
			// 合力の計算
			Vector3 force = restoringForce + kGravity + ball_.moveVector;

			// 速度と位置の更新
			ball_.acceleration = force / ball_.mass;
			ball_.velocity += (ball_.acceleration) * deltaTime;
			ball_.mass += 1.0f;

			// 張力方向に沿った痩躯度成分を除去して振り子運動を維持
			Vector3 velocityAlongRope;
			float dotProduct = ball_.velocity.x * direction.x + ball_.velocity.y * direction.y + ball_.velocity.z * direction.z;
			float lengthSq = direction.Length();
			lengthSq = lengthSq * lengthSq;
			if (lengthSq > 0.0f) {
				velocityAlongRope = direction * (dotProduct / lengthSq);
			}

			// 速度と移動ベクトルの計算
			ball_.velocity = ball_.velocity - velocityAlongRope;
			moveVec_ = ball_.velocity * deltaTime;

			// 常に自然長を保つ
			Vector3 toAnchor = (moveVec_ + playerPos) - anchor_;
			float newLength = toAnchor.Length();

			// スイングのアーチ状の動きを保持するために、ウェブの長さを自然長に制約
			if (newLength > naturalLength_) {
				// 長さを調整し、スイングの円運動を保つ
				Vector3 resultPos = (toAnchor.Normalize() * naturalLength_);
				resultPos += anchor_;
				moveVec_ = resultPos - playerPos ;
			}

			// 速度が閾値以下なら滞空状態に入る
			float leg = ball_.velocity.Length();
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

#ifdef _DEBUG
	ImGui::Begin("WebSwing");
	ImGui::Text("NaturalLength %f", naturalLength_);
	ImGui::Text("nowLength %f", ((moveVec_ + playerPos) - anchor_).Length());
	ImGui::DragFloat("mass", &ball_.mass, 0.1f);
	ImGui::DragFloat("stiffness", &stiffness_, 0.1f);
	ImGui::End();
#endif // _DEBUG

	
	springLine_->SetLine(anchor_, moveVec_ + playerPos);
	flag = isSwing_;
	
    return moveVec_;
}

Vector3 WebSwing::ReleaseWeb(PlayerParam& ball, Vector3 anchor, Vector3& swingDirection) {
	const float jumpStrength = 3.0f;  // ジャンプ初速の強さ
	const float airDamping = 1.0f;    // 空中での減衰効果

	// スイング方向に基づいて初速を設定
	Vector3 jumpVelocity = swingDirection * jumpStrength;
	ball.velocity = jumpVelocity;

	// 空中挙動（ジャンプ中の更新処理）
	return ApplyAirMovement(ball, airDamping);
}

Vector3 WebSwing::ApplyAirMovement(PlayerParam& ball, float airDamping) {
	const float deltaTime = 1.0f;
	static const Vector3 kGravity(0.0f, -0.00098f, 0.0f);

	// 空中での減衰と重力の影響
	ball.velocity *= airDamping;
	ball.velocity += kGravity;
	moveVec_ = ball.velocity;

	return moveVec_;
}