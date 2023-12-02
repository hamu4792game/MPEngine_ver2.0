#pragma once

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>

#include <wrl.h>
#include <cstdint>

#pragma comment(lib,"dinput8.lib")

class KeyInput
{
public:
	KeyInput() = default;
	~KeyInput() = default;

	static KeyInput* GetInstance();
	void Initialize();
	void Update();

private:
	//	DirectInputの初期化
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
	//	キーボードの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;
	//	キーの用意
	BYTE key[256] = {};
	BYTE oldKey[256] = {};

public:	// キー
	// キーが押されているか
	bool PressKey(uint8_t keynumber);

	//	キーを押した瞬間
	bool TriggerKey(uint8_t keynumber);

	//	キーを離した瞬間
	bool ReleaseKey(uint8_t keynumber);

};
