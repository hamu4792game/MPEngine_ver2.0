#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <string>
#include <memory>

// 衝突判定用の前方宣言
class AABB;

class Collider {
public:
	Collider() = default;
	~Collider() = default;

	enum Type {
		Box,
		Sphere,

		kMaxNum
	};

	void Initialize(const WorldTransform& transform, Type type); // 初期化処理
	void Update(); // 更新処理
	
	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="coll">コライダー</param>
	/// <param name="pushbackVec">押し戻しベクトル</param>
	/// <returns>true:当たっている/false:当たっていない</returns>
	bool OnCollision(const Collider& coll, Vector3& pushbackVec);

private:

	// 箱用の処理
	bool BoxCollision(const Collider& coll, Vector3& pushbackVec);

private:
	std::string name_; // コライダーの名前
	std::shared_ptr<AABB> boxCollider_;
	Type collderType_;
	WorldTransform transform_;


};
