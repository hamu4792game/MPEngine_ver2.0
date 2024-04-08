#include "MouseInput.h"
#include <Base/WindowSupervisor/WindowSupervisor.h>
#include <cassert>

void MouseInput::Initialize() {
	HRESULT result;
	// DirectInputの初期化
	directInput = nullptr;
	result = DirectInput8Create(WindowSupervisor::GetInstance()->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	mouse = nullptr;
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, nullptr);
	assert(SUCCEEDED(result));
	// 入力データの形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse);
	//	排他制御レベルのセット
	result = mouse->SetCooperativeLevel(WindowSupervisor::GetInstance()->GetHwnd(),
		DISCL_FOREGROUND |		/*画面が手前にある場合のみ入力を受け付ける*/
		DISCL_NONEXCLUSIVE |	/*デバイスをこのアプリだけで専有しない*/
		DISCL_NOWINKEY);		/*Windowsキーを無効にする*/
	assert(SUCCEEDED(result));
}

void MouseInput::Update() {
	// マウスの情報を取得する
	mouse->Acquire();
	// 前情報の入力
	oldMouseState = mouseState;
	// 今のキー状態の取得
	mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState.state);

	// 座標の取得
	POINT p{};
	GetCursorPos(&p);
	ScreenToClient(WindowSupervisor::GetInstance()->GetHwnd(), &p);
	mouseState.position = p;
}

bool MouseInput::PressMouse(MouseButton mousenumber) const {
	uint32_t handle = static_cast<uint32_t>(mousenumber);
	return mouseState.state.rgbButtons[handle];
}

bool MouseInput::TriggerMouse(MouseButton mousenumber) const {
	uint32_t handle = static_cast<uint32_t>(mousenumber);
	return mouseState.state.rgbButtons[handle] && !oldMouseState.state.rgbButtons[handle];
}

bool MouseInput::ReleaseMouse(MouseButton mousenumber) const {
	uint32_t handle = static_cast<uint32_t>(mousenumber);
	return !mouseState.state.rgbButtons[handle] && oldMouseState.state.rgbButtons[handle];
}

Vector2 MouseInput::GetScreenPosition(bool now) const {
	Vector2 vec;
	if (now) {
		vec.x = static_cast<float>(mouseState.position.x);
		vec.y = static_cast<float>(mouseState.position.y);
		return vec;
	}
	vec.x = static_cast<float>(oldMouseState.position.x);
	vec.y = static_cast<float>(oldMouseState.position.y);
	return vec;
}

Vector2 MouseInput::GetMouseMove() const {
	Vector2 vec;
	vec.x = static_cast<float>(mouseState.state.lX);
	vec.y = static_cast<float>(mouseState.state.lY);
	return vec;
}

float MouseInput::GetMouseWheel() const {
	return static_cast<float>(mouseState.state.lZ);
}
