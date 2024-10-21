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

	stiffness_ = 100.0f;
	dampingCoefficient_ = 2.0f;

}

void WebSwing::SetWeb(const Vector3& target, const Vector3& player) {
    target_ = target;
    Vector3 vec = FindVector(player, target_);
	float distance = Length(vec);
    moveVec_ = vec.Normalize();
    maxLength_ = Length(vec); // ロープの長さ。最初にセットしたタイミングで決まる
}

bool WebSwing::Update(const Vector3& player, Vector3& result) {

	// playerとtargetのベクトルを求めて加速、playerの座標を動かすのではなく、移動ベクトルを返す
    
    float distance = Length(FindVector(player, target_)); // 今の距離の長さ、更新処理で取得する
    Vector3 direction = FindVector(player, target_).Normalize(); // 向きベクトル
    const float g = 9.81f; // 重力加速度

    // あとで、逆処理を追加する
    // 距離がロープの長さを超えた場合、スイング運動を計算
    if (distance >= maxLength_) {
        // 張力による力を計算
        Vector3 tensionForce = direction * (distance - maxLength_) * g;

        // 重力による力 下向き重力の定義
        Vector3 gravityForce(0.0f, g, 0.0f);

        // 合力
        Vector3 totalForce = tensionForce + gravityForce;

        // 加速度 = 力 / 質量 (質量は1と仮定)
        Vector3 acceleration = totalForce;

        // 速度と位置の更新
        moveVec_ = acceleration.Normalize();
        result = moveVec_;
    }
    else {
        // それ以外なら、前回の移動ベクトルを返し、処理を終了する
        result = moveVec_;
        return true;
    }
    return false;
}

Vector3 WebSwing::Update(const Vector3& playerPos) {
	const float deltaTime = 1.0f / 60.0f;
	static const Vector3 kGravity(0.0f, -9.8f, 0.0f);
	
	if (!isSwing_) {
		moveVec_ = ball_.velocity * deltaTime;
	}
	else {
		Vector3 diff = playerPos - anchor_;
		float length = Length(diff);
		if (length > 0.0f) {
			Vector3 direction = diff.Normalize();
			// ウェブの張力（アンカーに引っ張られる力）
			Vector3 restoringForce = direction * (-stiffness_ * (length - naturalLength_));
			// ダンピングの計算（減衰力）
			Vector3 dampingForce = ball_.velocity * -dampingCoefficient_;
			// 合力の計算
			Vector3 force = restoringForce + kGravity + ball_.moveVector;

			// 速度と位置の更新
			ball_.acceleration = force / ball_.mass;
			ball_.velocity += (ball_.acceleration) * deltaTime;

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
			moveVec_ += ball_.velocity * deltaTime;

			// 常に自然長を保つ
			Vector3 toAnchor = playerPos - anchor_;
			float newLength = toAnchor.Length();

			// スイングのアーチ状の動きを保持するために、ウェブの長さを自然長に制約
			if (newLength > naturalLength_) {
				// 長さを調整し、スイングの円運動を保つ
				Vector3 resultPos = anchor_ + toAnchor.Normalize() * naturalLength_;
				moveVec_ = resultPos - playerPos;
			}
		}
	}
	springLine_->SetLine(anchor_, playerPos + moveVec_);
	
    return moveVec_;
}
