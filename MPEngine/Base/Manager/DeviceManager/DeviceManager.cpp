#include "DeviceManager.h"
#include "MPEngine/Base/Log.h"
#include <cassert>

DeviceManager* DeviceManager::GetInstance() {
	static DeviceManager instance;
	return &instance;
}

void DeviceManager::CreateDevice(IDXGIAdapter4* useAdapter) {
	device_ = nullptr;
	//	機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//	高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//	採用したアダプタでデバイスを生成
		HRESULT hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));
		//	指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			//	生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//	デバイスの生成がうまくいかなかったので機能できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");	//初期化完了のログを出す
}
