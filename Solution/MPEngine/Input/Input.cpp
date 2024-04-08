#include "Input.h"
#include "Audio.h"

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	key = std::make_unique<KeyInput>();
	pad = std::make_unique<PadInput>();
	mouse = std::make_unique<MouseInput>();

	key->Initialize();
	pad->Initialize();
	mouse->Initialize();
	MasterAudio::GetInstance()->Intialize();
}

void Input::Update() {
	key->Update();
	pad->Update();
	mouse->Update();
}

