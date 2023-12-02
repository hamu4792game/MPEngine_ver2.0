#include "Input.h"
#include "KeyInput.h"
#include "PadInput.h"

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	key = KeyInput::GetInstance();
	pad = PadInput::GetInstance();

	key->Initialize();
	pad->Initialize();
}

void Input::Update() {
	key->Update();
	pad->Update();
}

