#include "CommandDirectX.h"

CommandDirectX* CommandDirectX::GetInstance() {
	static CommandDirectX instance;
	return &instance;
}
