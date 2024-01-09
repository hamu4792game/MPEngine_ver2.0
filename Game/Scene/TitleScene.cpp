#include "TitleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"

void TitleScene::Initialize() {
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize();
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	auto input = Input::GetInstance();
	// 終了処理
	bool flag = false;
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		flag = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			flag = true;
		}
	}
	if (flag) {
		endRequest_ = true;
	}

	titleUI_->Update();

	cameraTransform_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTransform_);
}

void TitleScene::DrawImGui() {

}
