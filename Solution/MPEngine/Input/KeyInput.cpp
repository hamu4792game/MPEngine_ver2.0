#include "KeyInput.h"
#include <cassert>
#include "MPEngine/Base/WindowSupervisor/WindowSupervisor.h"

void KeyInput::Initialize() {
	HRESULT result;
	// DirectInputの初期化
	directInput = nullptr;
	result = DirectInput8Create(WindowSupervisor::GetInstance()->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードの生成
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データの形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(WindowSupervisor::GetInstance()->GetHwnd(),
		DISCL_FOREGROUND |		/*画面が手前にある場合のみ入力を受け付ける*/
		DISCL_NONEXCLUSIVE |	/*デバイスをこのアプリだけで専有しない*/
		DISCL_NOWINKEY);		/*Windowsキーを無効にする*/
	assert(SUCCEEDED(result));
}

void KeyInput::Update() {
	// キーボード情報の取得開始
	// 全キーの入力状態を取得する
	keyboard->Acquire();
	// 前フレームのキー状態の取得
	for (uint16_t i = 0; i < 256; i++) {
		oldKey[i] = key[i];
	}
	// 今のキー状態の取得
	keyboard->GetDeviceState(sizeof(key), key);
}

bool KeyInput::PressKey(uint8_t keynumber) const {
	return key[keynumber];
}

bool KeyInput::TriggerKey(uint8_t keynumber) const {
	return key[keynumber] && !oldKey[keynumber];
}

bool KeyInput::ReleaseKey(uint8_t keynumber) const {
	return !key[keynumber] && oldKey[keynumber];
}