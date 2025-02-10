#include "GameScene.h"

#include "Input/Input.h"
#include "ResourceManager/ResourceManager.h"

#include "NT/Server.h"
#include "NT/Client.h"

void GameScene::Initialize() {
	int result = MessageBox(NULL, L"サーバーですか？", L"確認", MB_YESNO | MB_ICONQUESTION);

	if (result == IDYES) {
		network_ = std::make_unique<Server>();
		isServer_ = true;
	}
	else {
		network_ = std::make_unique<Client>();
		isServer_ = false;
	}

	network_->SetThread();

	auto rs = ResourceManager::GetInstance();

	float pos[2]{
		-100.0f,100.0f
	};

	int index = 0;
	for (auto& play : players_) {
		play = std::make_unique<Player>();
		play->texture_ = std::make_unique<Sprite>();
		play->texture_->SetTexture(rs->FindTexture("Circle"));
		play->radius_ = 100.0f;
		play->pos_ = Vector2(pos[index], 0.0f);
		index++;
	}

	// 自分が選択している側の色を赤に
	//players_.at(static_cast<int>(isServer_))->texture_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

}

void GameScene::Finalize() {
	network_->Finalize();
}

void GameScene::Update() {
	auto& player = players_.at(static_cast<int>(isServer_));
	auto& enemy = players_.at(static_cast<int>(!isServer_));
	player->Update();

	if (isServer_) {
		// サーバー側なら
		NetworkData::Data data;
		data = network_->GetData();
		data.posA = player->pos_;
		network_->SetData(data);
		enemy->pos_ = data.posB;
	}
	else {
		// クライアント側なら
		NetworkData::Data data;
		data = network_->GetData();
		data.posB = player->pos_;
		network_->SetData(data);
		enemy->pos_ = data.posA;
	}

	SecondUpdate();
}

void GameScene::SecondUpdate() {
	// 通信先の更新 // サーバー側
	auto& player = players_.at(1u);
	auto& enemy = players_.at(0u);
	
	// 衝突判定
	player->IsCollition(enemy->pos_, enemy->radius_);
	for (auto& pl : players_) {
		// 描画更新
		pl->texture_->SetTranslate(pl->pos_);
		pl->texture_->SetScale(Vector2(pl->radius_, pl->radius_));
	}
}

GameScene::Player::Player() {
	const int kMaxCount = 30;
	count_ = kMaxCount;
}

void GameScene::Player::Update() {
	auto input = Input::GetInstance()->GetKey();
	if (input->TriggerKey(DIK_SPACE)) {
		count_ -= 1;
	}
	const float speed = 1.0f;
	// キー入力
	if (input->PressKey(DIK_LEFTARROW)) {
		pos_.x -= speed;
	}
	if (input->PressKey(DIK_RIGHTARROW)) {
		pos_.x += speed;
	}
	if (input->PressKey(DIK_UPARROW)) {
		pos_.y += speed;
	}
	if (input->PressKey(DIK_DOWNARROW)) {
		pos_.y -= speed;
	}
	
}

void GameScene::Player::IsCollition(const Vector2& position, const float& rad) {
	texture_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	float diffA = position.x - this->pos_.x;              // Xの差
	float diffB = position.y - this->pos_.y;              // Yの差
	float c = (float)sqrt(diffA * diffA + diffB * diffB); // 平方根
	float sumRadius = this->radius_ + rad;              // 半径の和

	// cが半径の和以下なら当たっている
	if (c <= sumRadius * 0.5f) {
		// 当たってる
		texture_->SetColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}
}
