#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include <wrl.h>
#include <string>
#include <array>

class ShaderManager {
private:
	ShaderManager() = default;
	~ShaderManager() = default;
public:
	static ShaderManager* GetInstance();

	void DXcInitialize();

	enum ShaderType {
		Vertex,
		Pixel,
		Compute,

		kMaxNum, // カウント用
	};

private:
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	std::array<const wchar_t*, ShaderType::kMaxNum> profile_;

	const std::string kDirectlyPath = "./Resources/Shader/";

public:
	// コンパイル呼び出し
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::string& filePath, ShaderType type);

private:
	// 実際のシェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);


};
