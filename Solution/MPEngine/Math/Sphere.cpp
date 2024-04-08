#include "Sphere.h"
#include <numbers>

void Sphere::Initialize(const float& radius) {
	radius_ = radius;
}

void Sphere::Update(WorldTransform& transform) {
	transform.UpdateMatrix();
	center_ = transform.GetTranslate();
}

void Sphere::Draw(const Matrix4x4& viewProjection, uint32_t color) {
	const uint32_t kSubdivision = 8;
	//	経度分割1つ分の角度
	const float kLonEvery = std::numbers::pi_v<float> *2.0f / static_cast<float>(kSubdivision);
	//	緯度分割1つ分の角度
	const float kLatEvery = std::numbers::pi_v<float> / static_cast<float>(kSubdivision);

	int index = 0;

	//	緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++)
	{
		//	現在の緯度
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++)
		{
			//	現在の経度
			float lon = lonIndex * kLonEvery;

			//	world座標系でのabcを求める
			Vector3 a, b, c;
			a = { (cosf(lat) * cosf(lon)) * radius_ ,sinf(lat) * radius_,(cosf(lat) * sinf(lon)) * radius_ };
			b = { cosf(lat + kLatEvery) * cosf(lon) * radius_ ,sinf(lat + kLatEvery) * radius_ ,cosf(lat + kLatEvery) * sinf(lon) * radius_ };
			c = { cosf(lat) * cosf(lon + kLonEvery) * radius_ ,sinf(lat) * radius_ ,cosf(lat) * sinf(lon + kLonEvery) * radius_ };

			//	abで縦、acで横の線を引く
			lines_[index].DrawLine(a + center_, b + center_, viewProjection, color);
			index++;
			lines_[index].DrawLine(a + center_, c + center_, viewProjection, color);
			index++;
		}
	}
}
