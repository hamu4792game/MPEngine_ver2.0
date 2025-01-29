#pragma once
#include <memory>
#include "KeyInput.h"
#include "PadInput.h"
#include "MouseInput.h"

// 入力系統関数
class Input {
private:
	Input() = default;
	~Input() = default;
	Input(const Input& input) = delete;
	const Input& operator=(const Input& input) = delete;
public:
	static Input* GetInstance();
	void Initialize(); // 初期化処理
	void Update(); // 更新処理

	//　ゲッター
	KeyInput* GetKey() { return key.get(); }
	PadInput* GetPad() { return pad.get(); }
	MouseInput* GetMouse() { return mouse.get(); }

private: // 個々の入力クラス変数
	std::unique_ptr<KeyInput> key;
	std::unique_ptr<PadInput> pad;
	std::unique_ptr<MouseInput> mouse;

};
