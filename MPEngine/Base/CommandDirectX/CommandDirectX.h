#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class CommandDirectX
{
public:
	CommandDirectX();
	~CommandDirectX();

	static CommandDirectX* GetInstance();

private:

};
