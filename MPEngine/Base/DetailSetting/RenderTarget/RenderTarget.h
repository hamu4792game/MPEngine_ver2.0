#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget() = default;

private:
	//	RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	DescriptorHeap* rtvDescriptorHeap = nullptr;
};
