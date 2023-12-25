#pragma once
#include <memory>
#include "KeyInput.h"
#include "PadInput.h"

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

private:
	std::unique_ptr<KeyInput> key;
	std::unique_ptr<PadInput> pad;

};
