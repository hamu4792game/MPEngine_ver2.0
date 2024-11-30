#include "PointOfGazeSearch.h"
#include "MPEngine/Utils/Camera/Camera3d.h"
#include "MPEngine.h"
#include <limits>
#undef max
#include "ImGuiManager/ImGuiManager.h"

void PointOfGazeSearch::Initialize() {
    collision_ = std::make_unique<Collider>();
    collision_->Initialize(&collTrans_, Collider::Type::Line, "CameraRay");
    collision_->SetLineColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    lockOnMark_ = std::make_shared<Sprite>();
    lockOnMark_->SetTexture(ResourceManager::GetInstance()->FindTexture("Target"));

    lockOnMark_->SetScale(Vector2(64.0f, 64.0f));
}

Vector3* PointOfGazeSearch::Update(const std::list<std::shared_ptr<Ground>>& targets, const Vector3& playerPos, const WorldTransform& cameraTrans) {
	auto& camera = Camera3d::GetInstance()->GetTransform();
    Quaternion qua = Quaternion::FromRotationMatrix4x4(MakeRotateMatrix(cameraTrans.rotation_));
    direction_ = FindVector(cameraTrans.GetPosition(), playerPos).Normalize();
    lockOnMark_->SetIsActive(false);
    ImGui::DragFloat3("Direction", &direction_.x, 0.1f);

    // カメラの正面ベクトルを取得
    collTrans_.translation_ = playerPos;
    collTrans_.UpdateMatrix();
    WorldTransform trans;
    // diffのために長さを設定し送る
    trans.translation_ = collTrans_.GetPosition() + (direction_ * 100.0f);
    trans.UpdateMatrix();
    collision_->Update(trans);

    Search(targets);
    if (target_) {
        return target_;
    }
    return nullptr;
}

Vector2 PointOfGazeSearch::ChangeScreen(const Vector3& worldPos) {
    auto camera = Camera3d::GetInstance();
    //	ビューポート行列
    static const  Matrix4x4 matViewport = MakeViewportMatrix(0, 0, static_cast<float>(MPEngine::GetInstance()->windowWidth_), static_cast<float>(MPEngine::GetInstance()->windowHeight_), 0, 1);
    //	ビュー行列とプロジェクション行列、ビューポート行列を合成する
    Matrix4x4 matViewProjectionViewport = camera->GetViewProMat() * matViewport;
    //	ワールドからスクリーン座標へ変換（ここで2Dから3Dになる）
    Vector3 result = Transform(worldPos, matViewProjectionViewport);

    static const Matrix4x4 ortho = MakeOrthographicMatrix(-float(MPEngine::GetInstance()->windowWidth_ / 2), float(MPEngine::GetInstance()->windowHeight_ / 2),
        float(MPEngine::GetInstance()->windowWidth_ / 2), -float(MPEngine::GetInstance()->windowHeight_ / 2), 0.01f, 100.0f);
    Matrix4x4 in = Inverse(MakeIdentity4x4() * ortho * matViewport);

    result = Transform(result, in);
    return Vector2(result.x, result.y);
}

void PointOfGazeSearch::Search(const std::list<std::shared_ptr<Ground>>& targets) {
	auto camera = Camera3d::GetInstance();
    camera->GetViewProMat();

	// ロックオン対象の絞り込み
	targets_.clear();
    // 全ての敵に対してロックオン判定
    for (const std::shared_ptr<Ground>& target : targets) {
        // 範囲内にオブジェクトが存在する場合、登録する
        float distanceToCenter;
        if (DistanceCheck(target->GetTrans().GetPosition(), distanceToCenter)) {
            targets_.emplace_back(std::make_pair(distanceToCenter, target.get()));
        }
    }

    target_ = nullptr;
    if (!targets_.empty()) {
        // 距離で昇順にソート (カメラ中心からの距離で比較)
        targets_.sort([](auto& pair1, auto& pair2) {
            return pair1.first < pair2.first;
            });


        // オブジェクトの上の辺がカメラ内にあるかを見る
        for (auto& target : targets_) {
            // Y座標が上の部分を取得
            Vector3 pos = target.second->GetCollision()->GetTransform().GetPosition();
            Vector3 scale = target.second->GetCollision()->GetTransform().scale_;
            std::array<Vector3, 4> points{
                pos + scale,
                Vector3(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z),
                Vector3(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z),
                Vector3(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z),
            };

            // 最短距離の頂点を求める
            float minDistance = std::numeric_limits<float>::max();
            Vector3 minPoint;
            for (auto& point : points) {
                float dis = Distance(camera->GetTransform().GetPosition(), point);
                if (minDistance > dis) {
                    minDistance = dis;
                    minPoint = point;
                }
            }

            // その点が範囲内に存在するか調べる
            float handle;
            if (!DistanceCheck(minPoint, handle)) {
                // しない場合終了
                continue;
            }

            // カメラからのレイを飛ばし、最近接点を求める
            
            Vector3 pushBackVec;
            if (!collision_->OnCollision(*target.second->GetCollision(), pushBackVec)) {
                break;
            }

            // 取得したベクトルに始点を足して最近接点を求める
            Vector3 recentlyContact = (collTrans_.GetPosition() + (direction_ * 100.0f)) - pushBackVec;
            // 最近接点が求まったので、y座標を上に合わせる
            if (recentlyContact.y != points.front().y) {
                recentlyContact.y = points.front().y;
            }
            if (!DistanceCheck(recentlyContact, handle)) {
                break;
            }
            // ターゲット
            target_ = &recentlyContact;

            // ロックオン状態なら
            if (target_) {
                // ワールド座標からスクリーン座標に変換
                Vector2 positionScreen;
                positionScreen = ChangeScreen(recentlyContact);
                lockOnMark_->SetTranslate(positionScreen);
                static float rot = 0.0f;
                lockOnMark_->SetRotate(rot += 0.05f);
                lockOnMark_->SetIsActive(true);
            }
            ImGui::Text("TargetOn");
            break;
        }
    }
}

bool PointOfGazeSearch::DistanceCheck(Vector3 pos, float& distanceToCenter) const {
    auto camera = Camera3d::GetInstance();
    // ワールドからビューへ変換
    Matrix4x4 view = camera->GetCamera().GetViewMat();
    Vector3 positionView = Transform(pos, view);

    // カメラ中心(スクリーン座標の原点)からの距離を計算
    distanceToCenter = std::sqrtf(
        std::powf(positionView.x, 2.0f) + // x軸距離
        std::powf(positionView.y, 2.0f)  // y軸距離
    );
    // 距離条件チェック (Z方向が視錐台内か確認)
    // 距離条件チェック
    if (minDistance_.z <= positionView.z && positionView.z <= maxDistance_.z) {
        if (minDistance_.y <= positionView.y && positionView.y <= maxDistance_.y) {
            if (minDistance_.x <= positionView.x && positionView.x <= maxDistance_.x) {
                return true;
            }
        }
    }
    return false;
}
