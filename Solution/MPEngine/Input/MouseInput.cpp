#include "MouseInput.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
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
	// マウスカーソルの固定 最初にやらなければ移動量が取れないため
	if (isLockMouseCursorToCenter_) {
		UpLockCursor();
	}
	// カーソル描画の更新 1フレーム遅れるが気にすることではないので突っ張る
	UpShowCursor();

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

void MouseInput::UpLockCursor(){
	HWND lHwnd = WindowSupervisor::GetInstance()->GetHwnd();
	// ウィンドウのクライアント領域を取得
	RECT clientRect;
	GetClientRect(lHwnd, &clientRect);

	// クライアント領域の左上をスクリーン座標に変換
	POINT clientTopLeft = { clientRect.left, clientRect.top };
	ClientToScreen(lHwnd, &clientTopLeft);

	// クライアント領域の中心を計算
	int centerX = clientTopLeft.x + (clientRect.right - clientRect.left) / 2;
	int centerY = clientTopLeft.y + (clientRect.bottom - clientRect.top) / 2;

	// カーソルをウィンドウの中心に移動
	SetCursorPos(centerX, centerY);
}

void MouseInput::UpShowCursor() const {
	if (mouseState.isShowCursor_ == oldMouseState.isShowCursor_) {
		// 前回と状況が同じなら早期リターン
		return;
	}
	ShowCursor(mouseState.isShowCursor_);
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
