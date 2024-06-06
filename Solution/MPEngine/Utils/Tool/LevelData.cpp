#include "LevelData.h"
#include <fstream>
#include <cassert>
#undef max
#undef min

LevelData* LevelData::Load(const std::string fileName) {
	// 連結してフルパスを得る
	const std::string directry = "./Resources/LevelData/";
	const std::string fullpath = directry + fileName;

	// ファイルストリーム
	std::ifstream file;
	// ファイルを開く
	file.open(fullpath);
	if (file.fail()) {
		// ファイルオープン失敗
		assert(0);
	}

	// ファイルチェック-------------
	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;
	// 解凍
	file >> deserialized;
	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// オブジェクトの走査------------
	// レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();
	const int32_t kMaxObjectNum = 10000; // オブジェクト最大数
	levelData->objects.reserve(kMaxObjectNum); // 先に場所だけ作っておく
	
	// "object"のオブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		ObjectScan(levelData, object);
	}
	return levelData;
}

void LevelData::ObjectScan(LevelData* levelData, nlohmann::json& object, WorldTransform* parent) {

	// typeデータがなければオブジェクトじゃないのでassert
	assert(object.contains("type"));
	// 種別を取得
	std::string type = object["type"].get<std::string>();

	// 親ポインタ
	WorldTransform* parent_ptr = nullptr;

	// 種類ごとの処理
	// MESH
	if (type.compare("MESH") == 0) {
		// 要素追加
		levelData->objects.emplace_back(LevelData::ObjectData());
		// 今追加した要素の参照を得る
		LevelData::ObjectData& objectData = levelData->objects.back();

		if (object.contains("file_name")) {
			// ファイル名
			objectData.fileName = object["file_name"];
		}

		// トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];

		/*Blenderでは
		* Xが右 (ゲーム:XにBlender:Xを代入)
		* Yが奥 (ゲーム:ZにBlender:Yを代入)
		* Zが上 (ゲーム:YにBlender:Zを代入)
		*/

		// 平行移動
		objectData.transform.translation_.x = (float)transform["translation"][0];
		objectData.transform.translation_.y = (float)transform["translation"][2];
		objectData.transform.translation_.z = (float)transform["translation"][1];
		// 回転角
		objectData.transform.rotation_.x = -(float)transform["rotation"][0];
		objectData.transform.rotation_.y = -(float)transform["rotation"][2];
		objectData.transform.rotation_.z = -(float)transform["rotation"][1];
		// スケーリング
		objectData.transform.scale_.x = (float)transform["scaling"][0];
		objectData.transform.scale_.y = (float)transform["scaling"][2];
		objectData.transform.scale_.z = (float)transform["scaling"][1];

		// 親子関係
		if (parent) {
			objectData.transform.parent_ = parent;
		}
		parent_ptr = &objectData.transform;
		objectData.transform.UpdateMatrix();

		// コライダー : 後々colliderを作るため
		if (object.contains("collider")) {
			nlohmann::json& coll = object["collider"];
			// コライダーのタイプを取得
			objectData.colliderType = coll.at("type");
			// centerを取得
			Vector3 handle;
			handle.x = (float)coll.at("center").at(0);
			handle.y = (float)coll.at("center").at(2);
			handle.z = (float)coll.at("center").at(1);

			// sizeを取得
			handle.x = (float)coll.at("size").at(0);
			handle.y = (float)coll.at("size").at(2);
			handle.z = (float)coll.at("size").at(1);
		}

	}

	// オブジェクト走査を再帰関数にまとめ、再帰呼び出しで枝を走査する
	if (object.contains("children")) {
		for (nlohmann::json& object : object["children"]) {
			ObjectScan(levelData, object, parent_ptr);
		}
	}
}
