#pragma once
#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>
#include <cstdint>
#pragma comment(lib,"dinput8.lib")
#include "Math/MathUtl.h"


// Inputクラスのみで生成
class MouseInput {
public:
	MouseInput() = default;
	~MouseInput() = default;

	void Initialize();
	void Update();

	enum MouseButton {
		LEFT = 0,
		RIGHT,
		CENTER,
	};

private:
	//	DirectInputの初期化
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
	//	マウスの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse;

	struct MouseParam{
		DIMOUSESTATE2 state;
		POINT position;
		bool isShowCursor_ = true; // マウスカーソルを表示させるか
	};
	MouseParam mouseState;
	MouseParam oldMouseState;
	bool isLockMouseCursorToCenter_ = false; // マウスカーソルをwindowの中心に固定させるか否か

private:
	// マウスカーソルのロック 固定というよりは初期化に近い
	void UpLockCursor();
	// マウスカーソルの表示更新
	void UpShowCursor() const;

public:
	// マウスが押されているか
	bool PressMouse(MouseButton mousenumber) const;

	// マウスを押した瞬間
	bool TriggerMouse(MouseButton mousenumber) const;

	// マウスを離した瞬間
	bool ReleaseMouse(MouseButton mousenumber) const;

	/// <summary>
	/// マウスのスクリーン座標を取得
	/// </summary>
	/// <param name="now">true = 今の座標:false = 前の座標</param>
	/// <returns>スクリーン座標</returns>
	Vector2 GetScreenPosition(bool now = true) const;

	// 現在のマウスの移動量を取得
	Vector2 GetMouseMove() const;
	// マウスのホイール量を取得
	float GetMouseWheel() const;

	// マウスカーソルをwindowの中心に固定するか
	void SetLockCursorToCenter(const bool& flag) { isLockMouseCursorToCenter_ = flag; }

	void SetShowCursor(const bool& flag) { mouseState.isShowCursor_ = flag; }

};
