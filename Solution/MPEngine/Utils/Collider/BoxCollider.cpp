#include "BoxCollider.h"
#include <cfloat>
#include <array>
#include "Graphics/Line/Line.h"

BoxCollider::~BoxCollider() {

}

void BoxCollider::Update(const WorldTransform& transform) {
	// 回転行列を抽出
	Matrix4x4 rotateMat = NormalizeMakeRotateMatrix(transform.worldMatrix_);
	transform_ = transform;

	// 単位行列と同じなら、回転していないのでAABBを使う
	if (rotateMat == MakeIdentity4x4()) {
		type_ = Type::AABB;
		AABBUpdate();
	}
	else {
		type_ = Type::OBB;
		OBBUpdate();
	}
	type_ = Type::AABB;
	AABBUpdate();

}

void BoxCollider::LineUpdate(std::vector<std::shared_ptr<Line>> lines) {
	std::array<Vector3, 8> vertices;

	switch (type_) {
	case BoxCollider::Type::AABB:
		vertices[0] = -aabb_.size; // 左上前
		vertices[1] = Vector3(aabb_.size.x, -aabb_.size.y, -aabb_.size.z); // 右上前
		vertices[2] = Vector3(-aabb_.size.x, -aabb_.size.y, aabb_.size.z); // 左上奥
		vertices[3] = Vector3(aabb_.size.x, -aabb_.size.y, aabb_.size.z); // 右上奥

		vertices[4] = Vector3(-aabb_.size.x, aabb_.size.y, -aabb_.size.z); // 左下前
		vertices[5] = Vector3(aabb_.size.x, aabb_.size.y, -aabb_.size.z); // 右下前
		vertices[6] = Vector3(-aabb_.size.x, aabb_.size.y, aabb_.size.z); // 左下奥
		vertices[7] = aabb_.size; // 右下奥
		break;
	case BoxCollider::Type::OBB:
		vertices[0] = -obb_.size; // 左上前
		vertices[1] = Vector3(obb_.size.x, -obb_.size.y, -obb_.size.z); // 右上前
		vertices[2] = Vector3(-obb_.size.x, -obb_.size.y, obb_.size.z); // 左上奥
		vertices[3] = Vector3(obb_.size.x, -obb_.size.y, obb_.size.z); // 右上奥

		vertices[4] = Vector3(-obb_.size.x, obb_.size.y, -obb_.size.z); // 左下前
		vertices[5] = Vector3(obb_.size.x, obb_.size.y, -obb_.size.z); // 右下前
		vertices[6] = Vector3(-obb_.size.x, obb_.size.y, obb_.size.z); // 左下奥
		vertices[7] = obb_.size; // 右下奥
		break;
	}
	// ワールド座標へ更新
	for (auto& ver : vertices) {
		ver += transform_.GetPosition();
	}
	
	// 描画
	// 上前
	lines[0]->SetLine(vertices[0], vertices[1]);
	// 上奥
	lines[1]->SetLine(vertices[2], vertices[3]);
	// 下前
	lines[2]->SetLine(vertices[4], vertices[5]);
	// 下奥
	lines[3]->SetLine(vertices[6], vertices[7]);
	// 左上
	lines[4]->SetLine(vertices[0], vertices[2]);
	// 左下
	lines[5]->SetLine(vertices[4], vertices[6]);
	// 右上
	lines[6]->SetLine(vertices[1], vertices[3]);
	// 右下
	lines[7]->SetLine(vertices[5], vertices[7]);
	// 左前
	lines[8]->SetLine(vertices[0], vertices[4]);
	// 左奥
	lines[9]->SetLine(vertices[2], vertices[6]);
	// 右前
	lines[10]->SetLine(vertices[1], vertices[5]);
	// 右奥
	lines[11]->SetLine(vertices[3], vertices[7]);

}

bool BoxCollider::IsCollision(const BoxCollider& coll, Vector3& minAxis, float& minOverlap) {
	bool flag = false;
	
	switch (type_) {
	case BoxCollider::Type::AABB:
		switch (coll.type_) {
		case BoxCollider::Type::AABB:
			flag = IsCollision(aabb_, coll.aabb_, minAxis, minOverlap);
			break;
		case BoxCollider::Type::OBB:
			break;
		}
		break;
	case BoxCollider::Type::OBB:
		switch (coll.type_) {
		case BoxCollider::Type::AABB:
			break;
		case BoxCollider::Type::OBB:
			flag = IsCollision(obb_, coll.obb_, minAxis, minOverlap);
			break;
		}
		break;
	}

	return flag;
}

void BoxCollider::AABBUpdate() {
	//	座標 - scale * size
	aabb_.size = MakeScale(transform_.worldMatrix_);
	aabb_.min = Vector3(transform_.GetPosition() - aabb_.size);
	aabb_.max = Vector3(transform_.GetPosition() + aabb_.size);
}

void BoxCollider::OBBUpdate() {
	obb_.center = transform_.GetPosition();
	obb_.size = MakeScale(transform_.worldMatrix_);

	// 回転行列を抽出
	Matrix4x4 rotateMat = NormalizeMakeRotateMatrix(transform_.worldMatrix_);
	obb_.orientations[0] = GetXAxis(rotateMat);
	obb_.orientations[1] = GetYAxis(rotateMat);
	obb_.orientations[2] = GetZAxis(rotateMat);
}

bool BoxCollider::IsCollision(const AABB& aabb1, const AABB& aabb2, Vector3& minAxis, float& minOverlap) {
	Vector3 aMin; Vector3 aMax;
	Vector3 bMin; Vector3 bMax;
	aMin.x = std::fminf(aabb1.min.x, aabb1.max.x); aMax.x = std::fmaxf(aabb1.min.x, aabb1.max.x);
	aMin.y = std::fminf(aabb1.min.y, aabb1.max.y); aMax.y = std::fmaxf(aabb1.min.y, aabb1.max.y);
	aMin.z = std::fminf(aabb1.min.z, aabb1.max.z); aMax.z = std::fmaxf(aabb1.min.z, aabb1.max.z);
	bMin.x = std::fminf(aabb2.min.x, aabb2.max.x); bMax.x = std::fmaxf(aabb2.min.x, aabb2.max.x);
	bMin.y = std::fminf(aabb2.min.y, aabb2.max.y); bMax.y = std::fmaxf(aabb2.min.y, aabb2.max.y);
	bMin.z = std::fminf(aabb2.min.z, aabb2.max.z); bMax.z = std::fmaxf(aabb2.min.z, aabb2.max.z);

	// 当たっていない場合、早期リターン
	if ((aMax.x < bMin.x || aMin.x > bMax.x) ||
		(aMax.y < bMin.y || aMin.y > bMax.y) ||
		(aMax.z < bMin.z || aMin.z > bMax.z)) {
		return false;
	}

	Vector3 overlap;
	// めり込み分を計算
	overlap.x = std::fminf(aMax.x, bMax.x) - std::fmaxf(aMin.x, bMin.x);
	overlap.y = std::fminf(aMax.y, bMax.y) - std::fmaxf(aMin.y, bMin.y);
	overlap.z = std::fminf(aMax.z, bMax.z) - std::fmaxf(aMin.z, bMin.z);

	if(overlap.x < overlap.y && overlap.x < overlap.z) {
		// X軸方向に押し戻し
		minAxis = { overlap.x, 0, 0 };
		minOverlap = overlap.x;
	}
	else if (overlap.y < overlap.x && overlap.y < overlap.z) {
		// Y軸方向に押し戻し
		minAxis = { 0, overlap.y, 0 };
		minOverlap = overlap.y;
	}
	else {
		// Z軸方向に押し戻し
		minAxis = { 0, 0, overlap.z };
		minOverlap = overlap.z;
	}

	return true;
}

bool BoxCollider::IsCollision(const AABB& aabb, const OBB& obb, Vector3& minAxis, float& minOverlap) {

	return false;
}

bool BoxCollider::IsCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap) {
	Vector3 separationAxisCandidate[15];
	// 各軸
	separationAxisCandidate[0] = obb1.orientations[0];
	separationAxisCandidate[1] = obb1.orientations[1];
	separationAxisCandidate[2] = obb1.orientations[2];
	separationAxisCandidate[3] = obb2.orientations[0];
	separationAxisCandidate[4] = obb2.orientations[1];
	separationAxisCandidate[5] = obb2.orientations[2];
	// 各辺のクロス積
	separationAxisCandidate[6] = Normalize(Cross(obb1.orientations[0], obb2.orientations[0]));
	separationAxisCandidate[7] = Normalize(Cross(obb1.orientations[0], obb2.orientations[1]));
	separationAxisCandidate[8] = Normalize(Cross(obb1.orientations[0], obb2.orientations[2]));
	separationAxisCandidate[9] = Normalize(Cross(obb1.orientations[1], obb2.orientations[0]));
	separationAxisCandidate[10] = Normalize(Cross(obb1.orientations[1], obb2.orientations[1]));
	separationAxisCandidate[11] = Normalize(Cross(obb1.orientations[1], obb2.orientations[2]));
	separationAxisCandidate[12] = Normalize(Cross(obb1.orientations[2], obb2.orientations[0]));
	separationAxisCandidate[13] = Normalize(Cross(obb1.orientations[2], obb2.orientations[1]));
	separationAxisCandidate[14] = Normalize(Cross(obb1.orientations[2], obb2.orientations[2]));
	// 頂点
	Matrix4x4 obb1WorldMatrix;
	obb1WorldMatrix .m = {
		obb1.orientations[0].x, obb1.orientations[0].y, obb1.orientations[0].z, 0.0f,
		obb1.orientations[1].x, obb1.orientations[1].y, obb1.orientations[1].z, 0.0f,
		obb1.orientations[2].x, obb1.orientations[2].y, obb1.orientations[2].z, 0.0f,
		obb1.center.x,          obb1.center.y,          obb1.center.z,          1.0f };
	Matrix4x4 obb2WorldMatrix;
	obb2WorldMatrix .m = {
		obb2.orientations[0].x, obb2.orientations[0].y, obb2.orientations[0].z, 0.0f,
		obb2.orientations[1].x, obb2.orientations[1].y, obb2.orientations[1].z, 0.0f,
		obb2.orientations[2].x, obb2.orientations[2].y, obb2.orientations[2].z, 0.0f,
		obb2.center.x,          obb2.center.y,          obb2.center.z,          1.0f };

	Vector3 vertices1[] = {
		-obb1.size,
		{obb1.size.x,  -obb1.size.y, -obb1.size.z},
		{obb1.size.x,  -obb1.size.y, obb1.size.z },
		{-obb1.size.x, -obb1.size.y, obb1.size.z },
		{-obb1.size.x, obb1.size.y,  -obb1.size.z},
		{obb1.size.x,  obb1.size.y,  -obb1.size.z},
		obb1.size,
		{-obb1.size.x, obb1.size.y,  obb1.size.z }
	};

	Vector3 vertices2[] = {
		-obb2.size,
		{obb2.size.x,  -obb2.size.y, -obb2.size.z},
		{obb2.size.x,  -obb2.size.y, obb2.size.z },
		{-obb2.size.x, -obb2.size.y, obb2.size.z },
		{-obb2.size.x, obb2.size.y,  -obb2.size.z},
		{obb2.size.x,  obb2.size.y,  -obb2.size.z},
		obb2.size,
		{-obb2.size.x, obb2.size.y,  obb2.size.z }
	};

	for (int i = 0; i < 8; i++) {
		vertices1[i] = obb1WorldMatrix * vertices1[i];
		vertices2[i] = obb2WorldMatrix * vertices2[i];
	}

	float minoverlap = FLT_MAX;
	int axisIndex = 0;

	// 各軸
	for (int i = 0; i < 15; i++) {
		if (separationAxisCandidate[i].x == 0.0f && separationAxisCandidate[i].y == 0.0f && separationAxisCandidate[i].z == 0.0f) {
			continue;
		}
		// 影の長さの合計
		float sumSpan;
		// 2つの影の両端の差分
		float longSpan;
		// 射影した最大値最小値
		float max1, min1, max2, min2;
		// 差分が形状を分離軸に射影した長さ
		float L1, L2;
		// すべての頂点を射影した値
		float Dot1[8];
		float Dot2[8];
		// 各頂点
		for (int j = 0; j < 8; j++) {
			Dot1[j] = Dot(separationAxisCandidate[i], vertices1[j]);
		}
		for (int k = 0; k < 8; k++) {
			Dot2[k] = Dot(separationAxisCandidate[i], vertices2[k]);
		}
		max1 = (std::max)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		min1 = (std::min)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		L1 = max1 - min1;
		max2 = (std::max)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		min2 = (std::min)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		L2 = max2 - min2;

		sumSpan = L1 + L2;
		longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

		if (sumSpan < longSpan) {
			return false;
		}
		float overlap = sumSpan - longSpan;
		if (overlap < minoverlap) {
			minoverlap = overlap;
			axisIndex = i;
		}
	}

	minAxis = separationAxisCandidate[axisIndex];
	minOverlap = minoverlap;

	return true;
}
