#include "Input.h"

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	key = std::make_unique<KeyInput>();
	pad = std::make_unique<PadInput>();

	key->Initialize();
	pad->Initialize();
}

void Input::Update() {
	key->Update();
	pad->Update();
}

