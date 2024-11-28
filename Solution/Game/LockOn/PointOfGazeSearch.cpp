#include "PointOfGazeSearch.h"
#include "MPEngine/Utils/Camera/Camera3d.h"
#include "MPEngine.h"

void PointOfGazeSearch::Initialize() {

}

void PointOfGazeSearch::Update(const std::list<std::shared_ptr<Ground>>& targets) {
	auto& camera = Camera3d::GetInstance()->GetTransform();

}

Vector2 PointOfGazeSearch::ChangeScreen(const Vector3& worldPos) {
	auto camera = Camera3d::GetInstance();
	//	ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, static_cast<float>(MPEngine::GetInstance()->windowWidth_), static_cast<float>(MPEngine::GetInstance()->windowHeight_), 0, 1);
	//	ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = camera->GetViewProMat() * matViewport;
	//	ワールドからスクリーン座標へ変換（ここで2Dから3Dになる）
	Vector3 result = Transform(worldPos, matViewProjectionViewport);

	Matrix4x4 ortho = MakeOrthographicMatrix(-float(MPEngine::GetInstance()->windowWidth_ / 2), float(MPEngine::GetInstance()->windowHeight_ / 2),
		float(MPEngine::GetInstance()->windowWidth_ / 2), -float(MPEngine::GetInstance()->windowHeight_ / 2), 0.01f, 100.0f);
	Matrix4x4 in = Inverse(MakeIdentity4x4() * ortho * matViewport);

	result = Transform(result, in);
	return Vector2(result.x, result.y);
}

void PointOfGazeSearch::Search(const std::list<std::shared_ptr<Ground>>& targets) {
	Camera3d::GetInstance()->GetViewProMat();
	// ロックオン対象の絞り込み
	targets_.clear();
    // 全ての敵に対してロックオン判定
    for (const std::shared_ptr<Ground>& target : targets) {

        Vector3 positionWorld = target->GetTrans().GetPosition();
        // ワールドからビューへ変換
        Matrix4x4 view = Camera3d::GetInstance()->GetCamera().GetViewMat();
        Vector3 positionView = Transform(positionWorld, view);

        // カメラ中心(スクリーン座標の原点)からの距離を計算
        float distanceToCenter = std::sqrtf(
            std::powf(positionView.x, 2.0f) + // x軸距離
            std::powf(positionView.y, 2.0f)  // y軸距離
        );

        // 距離条件チェック (Z方向が視錐台内か確認)
        if (minDistance_.z <= positionView.z && positionView.z <= maxDistance_.z) {
            targets_.emplace_back(std::make_pair(distanceToCenter, target.get()));
        }
    }

    target_ = nullptr;
    if (!targets_.empty()) {
        // 距離で昇順にソート (カメラ中心からの距離で比較)
        targets_.sort([](auto& pair1, auto& pair2) {
            return pair1.first < pair2.first;
            });

        // ソートの結果一番近い敵をロックオン対象とする
        target_ = targets_.front().second;
    }
}

void PointOfGazeSearch::SearchPoint() {

}
