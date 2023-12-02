#include "PadInput.h"
#include <limits>

PadInput* PadInput::GetInstance() {
	static PadInput instance;
	return &instance;
}

void PadInput::Initialize() {
	//	変数の初期化 指定した分だけ0にするZeroMemory
	ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
	//	DWORDは32bitのuint型 XInputと型を合わせている 
	DWORD dr = XInputGetState(0, &xInputState);
	//	接続があればのフラグ
	dr == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
}

void PadInput::Update() {
	//	キーの再取得
	oldXInputState = xInputState;
	DWORD dresult = XInputGetState(0, &xInputState);
	//	接続状況の確認
	dresult == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
	if (isConnectPad) {
		// デッドzoneの設定
		if ((xInputState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			xInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(xInputState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				xInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			xInputState.Gamepad.sThumbLX = 0;
			xInputState.Gamepad.sThumbLY = 0;
		}

		if ((xInputState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			xInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(xInputState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				xInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			xInputState.Gamepad.sThumbRX = 0;
			xInputState.Gamepad.sThumbRY = 0;
		}
	}
}

bool PadInput::GetPadConnect() {
	return isConnectPad;
}

bool PadInput::GetPadButton(UINT button) {
	return xInputState.Gamepad.wButtons == button;
}

bool PadInput::GetPadButtonUp(UINT button) {
	return xInputState.Gamepad.wButtons != button && oldXInputState.Gamepad.wButtons == button;
}

bool PadInput::GetPadButtonDown(UINT button) {
	return xInputState.Gamepad.wButtons == button && oldXInputState.Gamepad.wButtons != button;
}

Vector2 PadInput::GetPadLStick() {
	SHORT x = xInputState.Gamepad.sThumbLX;
	SHORT y = xInputState.Gamepad.sThumbLY;
	static constexpr float kNormal = 1.0f / static_cast<float>(std::numeric_limits<short>::max());

	return Vector2(static_cast<float>(x) * kNormal, static_cast<float>(y) * kNormal);
}

Vector2 PadInput::GetPadRStick() {
	SHORT x = xInputState.Gamepad.sThumbRX;
	SHORT y = xInputState.Gamepad.sThumbRY;
	static constexpr float kNormal = 1.0f / static_cast<float>(std::numeric_limits<short>::max());

	return Vector2(static_cast<float>(x) * kNormal, static_cast<float>(y) * kNormal);
}

bool PadInput::GetLTriggerDown() {
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bLeftTrigger < 128 && xInputState.Gamepad.bLeftTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool PadInput::GetRTriggerDown() {
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bRightTrigger < 128 && xInputState.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}
bool PadInput::GetLTrigger() {
	//	デッドラインの設定必須
	if (xInputState.Gamepad.bLeftTrigger >= 128) {
		return true;
	}
	return false;
}

bool PadInput::GetRTrigger() {
	//	デッドラインの設定必須
	if (xInputState.Gamepad.bRightTrigger >= 128) {
		return true;
	}
	return false;
}
