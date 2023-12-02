#pragma once

class KeyInput;
class PadInput;

class Input {
private:
	Input() = default;
	~Input() = default;

public:
	static Input* GetInstance();
	void Initialize();
	void Update();

	KeyInput* GetKey() { return key; }
	PadInput* GetPad() { return pad; }

private:
	KeyInput* key = nullptr;
	PadInput* pad = nullptr;

};
