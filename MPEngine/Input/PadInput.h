#pragma once

#include <Windows.h>
#undef max
#undef min
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "math/Vector2.h"

class PadInput
{
public:
	PadInput() = default;
	~PadInput() = default;

	static PadInput* GetInstance();
	void Initialize();
	void Update();

private:
	//	コントローラーの用意
	XINPUT_STATE xInputState = {};
	XINPUT_STATE oldXInputState = {};
	bool isConnectPad = false;

public: // コントローラー
	//パッドに接続されてるか
	bool GetPadConnect();

	//パッドのボタンが押されているか
	bool GetPadButton(UINT button);

	//パッドのボタンが離れた瞬間か
	bool GetPadButtonUp(UINT button);

	//パッドのボタンが押された瞬間か
	bool GetPadButtonDown(UINT button);

	//パッドの左スティック
	Vector2 GetPadLStick();

	//パッドの右スティック
	Vector2 GetPadRStick();

	//左トリガーを押し込んだ瞬間か
	bool GetLTriggerDown();

	//右トリガーを押し込んだ瞬間か
	bool GetRTriggerDown();

	//左トリガーが押されているか
	bool GetLTrigger();

	//右トリガーが押されているか
	bool GetRTrigger();

};
