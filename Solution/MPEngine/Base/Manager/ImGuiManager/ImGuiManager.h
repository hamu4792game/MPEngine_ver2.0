#pragma once

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG
#include "d3d12.h"

class ImGuiManager {
private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
public:
	static ImGuiManager* GetInstance();

	void Initialize(int bufferCount);
	void Finalize();
	void Begin();
	void End();
	void Draw(ID3D12GraphicsCommandList* list);

private:

};
