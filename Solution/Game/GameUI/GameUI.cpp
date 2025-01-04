#include "GameUI.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#include "Input/Input.h"
#include "Utils/Easing/Ease.h"

void GameUI::Initialize(const uint32_t& collectionNum) {
	auto rsManager = ResourceManager::GetInstance();
	auto global = GlobalVariables::GetInstance();
	global->CreateGroup("GameUI");
	global->LoadFiles();

	SetGlobalVariables();

	int index = 0;
	for (auto& hud : huds_) {
		hud = std::make_shared<Sprite>();
		hud->SetTexture(rsManager->FindTexture(kTextureNames_.at(index)));
		hud->SetScale(hudParams_.at(index).scale);
		hud->SetTranslate(hudParams_.at(index).translate);
		hud->SetUVSize(hudParams_.at(index).uvScale);
		hud->SetUVPosition(hudParams_.at(index).uvTranslate);
		index++;
	}

	collectionObjects_.resize(collectionNum);
	index = 0;
	for (auto& object : collectionObjects_) {
		object = std::make_unique<Sprite>();
		object->SetTexture(rsManager->FindTexture("CollectionObjectUI"));
		object->SetScale(collectionObjectsParameter_.hudParam.scale);
		Vector2 translate = collectionObjectsParameter_.hudParam.translate + Vector2(collectionObjectsParameter_.hudParam.scale.x * index++, 0.0f);
		object->SetTranslate(translate);
	}

	backGround_ = std::make_shared<Sprite>();
	backGround_->SetTexture(rsManager->FindTexture("white2x2"));
	backGround_->SetScale(Vector2(1280.0f, 720.0f));
	backGround_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));
	backGround_->SetLayerNum(0u);
	backGround_->SetIsActive(false);

}

void GameUI::Update(const uint32_t& collectionNum) {
	ImGuiProc();
	// 今の数と収集物の数が変わったときに更新
	if (collectionNum != oldCollectionobjectNum_) {
		CollectionObjectActiveUpdate(collectionNum);
	}
	oldCollectionobjectNum_ = collectionNum;

	// 入力があったらかわるよってやつ
	if (Input::GetInstance()->GetMouse()->PressMouse(MouseInput::LEFT)) {
		huds_.at(UniqueHud::Mouse)->SetUVPosition(Vector2(0.5f, 0.0f));
	}
	else {
		huds_.at(UniqueHud::Mouse)->SetUVPosition(Vector2::zero);
	}

}

bool GameUI::FirstUpdate(const uint32_t& collectionNum) {
	// でっかくして画面に見せる。あとでパラメーターは外部ファイルに入れる
	static const float kMagnification = 2.0f;
	static const float kMaxFrame = 120.0f;

	backGround_->SetIsActive(true);

	Vector2 bigScale = collectionObjectsParameter_.hudParam.scale * kMagnification;
	Vector2 bigTranslate;
	// objectの数の半分を画面左にずらす
	bigTranslate.x = -((collectionObjects_.size() * 0.5f) * bigScale.x - (bigScale.x * 0.5f));

	// ease準備
	Vector2 preScale,postScale;
	Vector2 preTranslate,postTranslate;
	if (!isScale_) {
		// 拡大
		preScale = collectionObjectsParameter_.hudParam.scale;
		postScale = bigScale;
		preTranslate = collectionObjectsParameter_.hudParam.translate;
		postTranslate = bigTranslate;
	}
	else {
		// 縮小
		preScale = bigScale;
		postScale = collectionObjectsParameter_.hudParam.scale;
		preTranslate = bigTranslate;
		postTranslate = collectionObjectsParameter_.hudParam.translate;
	}

	// ここから上はそのうち初期化にまとめる-------------------------------

	// 入力があればスキップ
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		nowFrame_ = kMaxFrame;
	}
	if (input->GetMouse()->TriggerMouse(input->GetMouse()->LEFT)) {
		nowFrame_ = kMaxFrame;
	}

	// ease更新
	Vector2 resultScale = Ease::UseEase(preScale, postScale, nowFrame_, kMaxFrame, Ease::EaseType::EaseInSine);
	Vector2 resultTranslate = Ease::UseEase(preTranslate, postTranslate, nowFrame_, kMaxFrame, Ease::EaseType::EaseInSine);

	// 座標更新
	int index = 0;
	for (auto& object : collectionObjects_) {
		object->SetScale(resultScale);
		Vector2 lTranslate = resultTranslate + Vector2(resultScale.x * index++, 0.0f);
		object->SetTranslate(lTranslate);
	}

	// 時間の加算
	nowFrame_++;
	if (nowFrame_ >= kMaxFrame) {
		// フラグの逆転
		nowFrame_ = 0.0f;
		isScale_ = !isScale_;
		CollectionObjectActiveUpdate(collectionNum); // 更新
		if (!isScale_) {
			// 終わったのでtrueを返す
			backGround_->SetIsActive(false);
			return true;
		}
	}
	return false;
}

void GameUI::CollectionObjectActiveUpdate(const uint32_t& collectionNum) {
	for (uint32_t index = 0; index < collectionObjects_.size(); index++) {
		// 今残ってる収集物以上なら色変える
		if (collectionNum <= index) {
			collectionObjects_.at(collectionObjects_.size() - 1u - index)->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else {
			collectionObjects_.at(collectionObjects_.size() - 1u - index)->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}

void GameUI::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("UI");
	int index = 0;
	for (auto& hud : huds_) {
		if (ImGui::TreeNode(kTextureNames_.at(index).c_str())) {
			ImGui::DragFloat2("Scale", &hudParams_.at(index).scale.x, 1.0f);
			ImGui::DragFloat2("Translate", &hudParams_.at(index).translate.x, 1.0f);
			ImGui::DragFloat2("UVScale", &hudParams_.at(index).uvScale.x, 0.01f);
			ImGui::DragFloat2("UVTranslate", &hudParams_.at(index).uvTranslate.x, 0.01f);
			ImGui::TreePop();
		}
		hud->SetScale(hudParams_.at(index).scale);
		hud->SetTranslate(hudParams_.at(index).translate);
		hud->SetUVSize(hudParams_.at(index).uvScale);
		hud->SetUVPosition(hudParams_.at(index).uvTranslate);
		index++;
	}
	if (ImGui::TreeNode("CollectionObject")) {
		ImGui::DragFloat2("Scale", &collectionObjectsParameter_.hudParam.scale.x, 1.0f);
		ImGui::DragFloat2("Translate", &collectionObjectsParameter_.hudParam.translate.x, 1.0f);
		ImGui::DragFloat2("UVScale", &collectionObjectsParameter_.hudParam.uvScale.x, 0.01f);
		ImGui::DragFloat2("UVTranslate", &collectionObjectsParameter_.hudParam.uvTranslate.x, 0.01f);
		ImGui::DragFloat("Distance", &collectionObjectsParameter_.distance, 0.1f);
		ImGui::TreePop();
	}
	index = 0;
	for (auto& object : collectionObjects_) {
		object->SetScale(collectionObjectsParameter_.hudParam.scale);
		Vector2 translate = collectionObjectsParameter_.hudParam.translate + Vector2(collectionObjectsParameter_.hudParam.scale.x * index++, 0.0f);
		object->SetTranslate(translate);
		object->SetUVSize(collectionObjectsParameter_.hudParam.uvScale);
		object->SetUVPosition(collectionObjectsParameter_.hudParam.uvTranslate);
	}

	ImGui::End();

#endif // _DEBUG
}

void GameUI::SetGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	for (int index = 0; index < UniqueHud::kMaxNum; index++) {
		hudParams_.at(index).scale = gv->GetVector2Value(itemName_, "GameUI_" + std::to_string(index) + "_Scale");
		hudParams_.at(index).translate = gv->GetVector2Value(itemName_, "GameUI_" + std::to_string(index) + "_Translate");
		hudParams_.at(index).uvScale = gv->GetVector2Value(itemName_, "GameUI_" + std::to_string(index) + "_UVScale");
		hudParams_.at(index).uvTranslate = gv->GetVector2Value(itemName_, "GameUI_" + std::to_string(index) + "_UVTranslate");
	}

	collectionObjectsParameter_.hudParam.scale = gv->GetVector2Value(itemName_, "GameUI_CollectionObjectsParameter_Scale");
	collectionObjectsParameter_.hudParam.translate = gv->GetVector2Value(itemName_, "GameUI_CollectionObjectsParameter_Translate");
	collectionObjectsParameter_.hudParam.uvScale = gv->GetVector2Value(itemName_, "GameUI_CollectionObjectsParameter_UVScale");
	collectionObjectsParameter_.hudParam.uvTranslate = gv->GetVector2Value(itemName_, "GameUI_CollectionObjectsParameter_UVTranslate");
	collectionObjectsParameter_.distance = gv->GetFloatValue(itemName_, "GameUI_CollectionObjectsParameter_Distance");


}

void GameUI::AddGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	// hud
	for (int index = 0; index < UniqueHud::kMaxNum; index++) {
		gv->SetValue(itemName_, "GameUI_" + std::to_string(index) + "_Scale", hudParams_.at(index).scale);
		gv->SetValue(itemName_, "GameUI_" + std::to_string(index) + "_Translate", hudParams_.at(index).translate);
		gv->SetValue(itemName_, "GameUI_" + std::to_string(index) + "_UVScale", hudParams_.at(index).uvScale);
		gv->SetValue(itemName_, "GameUI_" + std::to_string(index) + "_UVTranslate", hudParams_.at(index).uvTranslate);
	}

	// 収集物
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_Scale", collectionObjectsParameter_.hudParam.scale);
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_Translate", collectionObjectsParameter_.hudParam.translate);
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_UVScale", collectionObjectsParameter_.hudParam.uvScale);
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_UVTranslate", collectionObjectsParameter_.hudParam.uvTranslate);
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_UVTranslate", collectionObjectsParameter_.hudParam.uvTranslate);
	gv->SetValue(itemName_, "GameUI_CollectionObjectsParameter_Distance", collectionObjectsParameter_.distance);


	gv->SaveFile(itemName_);
}
