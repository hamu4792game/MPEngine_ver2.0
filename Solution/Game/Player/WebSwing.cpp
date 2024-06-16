#include "WebSwing.h"

WebSwing::WebSwing() {

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
