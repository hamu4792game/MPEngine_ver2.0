#include <Windows.h>
#include "MPEngine/MPEngine.h"
#include "MPEngine/Framework/GameFrame.h"
#include "GameScene/GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	GameFrame* game = new GameScene;
	MPEngine::GetInstance()->Run(game);
	delete game;
	return 0;
}