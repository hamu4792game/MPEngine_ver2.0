#include <Windows.h>
#include "MPEngine/MPEngine.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	auto engine = MPEngine::GetInstance();

	return 0;
}