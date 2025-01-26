#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#pragma comment (lib, "WSock32.lib")

DWORD WINAPI ThreadFunc(LPVOID lpParam);

class BaseNetwork {
public:
	BaseNetwork() = default;
	~BaseNetwork() = default;


	void SetThread();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;

protected:

	void InitializeWinSock();

	void CreateSocket();

	void FinalizeWinSock(SOCKET socket);

protected:
	SOCKET socket_;

	const WORD wPort = 8000;

};
