#include <Windows.h>
#include "MPEngine/MPEngine.h"
#include "GameScene/GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	std::unique_ptr<GameFrame> game = std::make_unique<GameScene>();
	MPEngine::GetInstance()->Run(game.get());
	return 0;
}