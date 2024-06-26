#pragma once
#include <string>
#include "MPEngine/Utils/WorldTransform/WorldTransform.h"
#include "externals/nlohmann/json.hpp"
#include <vector>
#include <memory>

class LevelData {
public:
	LevelData() = default;
	~LevelData() = default;
	struct ObjectData {
		WorldTransform transform; //objectのtransform
		//本来ならばobjectのmodelFileNameとして使用。
		std::string fileName;
		// 現在はobjectのTypeName 敵ならEnemy Blockなら箱等...
		std::string typeName;
		struct Collider {
			Vector3 center;
			Vector3 size;
			std::string colliderType;
		};
		Collider collider;
	};

	// ステージデータのロード
	static LevelData* Load(const std::string fileName);

	std::vector<ObjectData> GetData() { return objects; }

private:
	static void ObjectScan(LevelData* levelData, nlohmann::json& object, WorldTransform* parent = nullptr);
	std::vector<ObjectData> objects;
};