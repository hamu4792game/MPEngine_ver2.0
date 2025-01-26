#include <Windows.h>
#include "MPEngine/MPEngine.h"
#include "GameScene/NTScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	std::unique_ptr<GameFrame> game = std::make_unique<NTScene>();
	MPEngine::GetInstance()->Run(game.get());
	return 0;
}