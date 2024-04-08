#include "ImGuiManager.h"

// imguiのinclude
#ifdef _DEBUG
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Base/Manager/DeviceManager/DeviceManager.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#endif // _DEBUG

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(int bufferCount) {
#ifdef _DEBUG
	auto winApp = WindowSupervisor::GetInstance();
	auto device = DeviceManager::GetInstance();
	auto srvHeap = ResourceManager::GetInstance()->GetSRVHeap();

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(device->GetDevice(),
		bufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap->GetDescriptorHeap(),
		srvHeap->GetCPUDescriptorHandle(0),
		srvHeap->GetGPUDescriptorHandle(0)
	);
#endif // _DEBUG
}

void ImGuiManager::Finalize() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG
}

void ImGuiManager::Begin() {
#ifdef _DEBUG
	// ImGuiにframeの始まりを伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::End() {
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render();
#endif // _DEBUG
}

void ImGuiManager::Draw(ID3D12GraphicsCommandList* list) {
#ifdef _DEBUG
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);
#endif // _DEBUG
}


