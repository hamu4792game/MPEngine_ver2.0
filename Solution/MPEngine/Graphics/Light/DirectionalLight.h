#pragma once
#include "MPEngine/Base/ConstantBuffer.h"
#include "Math/MathUtl.h"

class DirectionalLight {
public:
	DirectionalLight();
	~DirectionalLight() = default;

	void Update();

private:
	struct Directional{
		Vector4 color; // ライトの色
		Vector3 direction; // ライトの向き
		float intensity; // ライトの輝度
	};
public:
	ConstantBuffer<Directional> cDirectionLight_;

};