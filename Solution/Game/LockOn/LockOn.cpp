#include "LockOn.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Utils/Camera/Camera3d.h"
#include "MPEngine/MPEngine.h"

LockOn::LockOn() {
	
}

void LockOn::Initialize() {
	lockOnMark_ = std::make_shared<Sprite>();
	lockOnMark_->SetTexture(ResourceManager::GetInstance()->FindTexture("Target"));
	frame_ = std::make_shared<Sprite>();
	frame_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));

	lockOnMark_->SetScale(Vector2(64.0f, 64.0f));
	frame_->SetScale(Vector2(640.0f, 500.0f));
	framePos_ = Vector2(-150.0f, -150.0f);
	frame_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	frame_->SetIsActive(false);

	collision_ = std::make_unique<Collider>();
	collision_->Initialize(&collTrans_, Collider::Type::Line, "CameraRay");
	collision_->SetLineColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
}

void LockOn::Update(const std::list<std::shared_ptr<Target>>& targets) {
	

	Search(targets);

	// ロックオン状態なら
	if (target_) {
		// 敵のロックオン座標取得
		Vector3 positionWorld = target_->GetTransform().GetPosition();
		// ワールド座標からスクリーン座標に変換
		Vector2 positionScreen;
		positionScreen = ChangeScreen(positionWorld);
		lockOnMark_->SetTranslate(positionScreen);
		static float rot = 0.0f;
		lockOnMark_->SetRotate(rot += 0.05f);
		lockOnMark_->SetIsActive(true);

		collTrans_.translation_ = Camera3d::GetInstance()->GetTransform().GetPosition();
		collTrans_.UpdateMatrix();
		collision_->Update(target_->GetTransform());
	}
	else {
		lockOnMark_->SetIsActive(false);
	}
}

void LockOn::OnCollisionStageToRay(const Collider& coll) {
	Vector3 pushbackVec;
	if (collision_->OnCollision(coll, pushbackVec)) {
		lockOnMark_->SetIsActive(false);
		target_ = nullptr; 
	}
}

WorldTransform* LockOn::GetTargetTrans() const {
	if (target_) {
		return (WorldTransform*)&target_->GetTransform();
	}
	return nullptr;
}

Vector2 LockOn::ChangeScreen(const Vector3& worldPos) {
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
	return Vector2(result.x,result.y);
}

void LockOn::Search(const std::list<std::shared_ptr<Target>>& targets) {
	Camera3d::GetInstance()->GetViewProMat();
	// ロックオン対象の絞り込み
	targets_.clear();
	// 全ての敵に対してロックオン判定
	for (const std::shared_ptr<Target>& target : targets) {

		Vector3 positionWorld = target->GetTransform().GetPosition();
		// ワールドからビューへ変換
		Matrix4x4 view = Camera3d::GetInstance()->GetCamera().GetViewMat();
		Vector3 positionView = Transform(positionWorld, view);


		// 最小範囲と最大範囲
		Vector3 min = Vector3(-30.0f, -30.0f, 50.0f);
		Vector3 max = Vector3(30.0f, 30.0f, 200.0f);
		// 距離条件チェック
		if (min.x <= positionView.x && positionView.x <= max.x) {
			if (min.y <= positionView.y && positionView.y <= max.y) {
				if (min.z <= positionView.z && positionView.z <= max.z) {
					targets_.emplace_back(std::make_pair(positionView.z, target.get()));
				}
			}
		}
	}

	target_ = nullptr;
	if (targets_.size() != 0) {
		// 距離で昇順にソート
		targets_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
		// ソートの結果一番近い敵をロックオン対象とする
		target_ = targets_.front().second;
	}
}
