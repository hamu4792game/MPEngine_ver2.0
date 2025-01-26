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
		play->texture_->SetTexture(rs->FindTexture("white2x2"));
		play->texture_->SetScale(Vector2(100.0f, 100.0f));
		play->texture_->SetTranslate(Vector2(pos[index], 0.0f));
		index++;
	}

	// 自分が選択している側の色を赤に
	players_.at(static_cast<int>(isServer_))->texture_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	index = 0;
	float numPos[4]{
		-150.0f,-100.0f,100.0f,150.0f,
	};
	for (auto& num : numbers_) {
		num = std::make_unique<Sprite>();
		num->SetTexture(rs->FindTexture("number"));
		num->SetTranslate(Vector2(numPos[index], 100.0f));
		num->SetScale(Vector2(50.0f, 50.0f));
		num->SetUVSize(Vector2(1.0f / 10.0f, 1.0f));
		index++;
	}

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
		data.A = player->count_;
		network_->SetData(data);
		enemy->count_ = data.B;
	}
	else {
		// クライアントp側なら
		NetworkData::Data data;
		data = network_->GetData();
		data.B = player->count_;
		network_->SetData(data);
		enemy->count_ = data.A;
	}

	if (player->GetFinish() || enemy->count_ <= 0) {
		endRequest_ = true;
		// 勝利者を取得
	}
	SecondUpdate();
}

void GameScene::SecondUpdate() {
	// 通信先の更新
	auto& player = players_.at(static_cast<int>(!isServer_));
	
	if (player->count_ % 2 == 0) {
		player->texture_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	else {
		player->texture_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	SetNumber();
}

void GameScene::SetNumber() {

	static const float size = 1.0f / 10.0f;
	int index = 0;
	// ナンバーの0,1がplayer0　・ 2,3がplayer1
	for (auto& player : players_) {
		int num = player->count_;
		// 10の位
		int tens_place = (num / 10) & 0xf;
		numbers_.at(index)->SetUVPosition(Vector2(size * tens_place, 0.0f));
		index++;
		// 1の位
		int ones_place = (num % 10);
		numbers_.at(index)->SetUVPosition(Vector2(size * ones_place, 0.0f));
		index++;
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
	if (count_ <= 0) {
		isFinished_ = true;
	}
}