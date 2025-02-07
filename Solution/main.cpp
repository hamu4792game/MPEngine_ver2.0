#include <Windows.h>
#include "MPEngine/MPEngine.h"
#include "GameScene/MasterGame.h"
#include "GameScene/TestScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	std::unique_ptr<MPE::GameFrame> game = std::make_unique<MasterGame>();
	MPEngine::GetInstance()->Run(game.get());
	return 0;
}