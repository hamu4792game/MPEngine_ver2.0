#pragma once
#include <memory>
#include "KeyInput.h"
#include "PadInput.h"
#include "MouseInput.h"

class Input {
private:
	Input() = default;
	~Input() = default;

public:
	static Input* GetInstance();
	void Initialize();
	void Update();

	KeyInput* GetKey() { return key.get(); }
	PadInput* GetPad() { return pad.get(); }
	MouseInput* GetMouse() { return mouse.get(); }

private:
	std::unique_ptr<KeyInput> key;
	std::unique_ptr<PadInput> pad;
	std::unique_ptr<MouseInput> mouse;

};
