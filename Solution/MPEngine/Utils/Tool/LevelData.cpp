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
	for (nlohmann::json& obj : deserialized["objects"]) {
		ObjectScan(levelData, obj);
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

		if (object.contains("name")) {
			// typenameの取得
			objectData.typeName = object["name"];
		}
		if (object.contains("file_name")) {
			// ファイル名
			objectData.fileName = object["file_name"];
		}
		if (object.contains("texturepath")) {
			// テクスチャ名の取得
			objectData.textureName = object["texturepath"];
			std::string extension = ".png";
			// 拡張子が見つかった場合、その位置を取得
			size_t pos = objectData.textureName.find(extension);
			if (pos != std::string::npos) {
				// 拡張子の部分を削除
				objectData.textureName.erase(pos, extension.length());
			}
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

		// 回転角 Quaternionで取得し、Euler角に戻して渡す
		Quaternion quaternion;
		quaternion.x = -(float)transform["rotation"][0];
		quaternion.y = -(float)transform["rotation"][2];
		quaternion.z = -(float)transform["rotation"][1];
		quaternion.w = (float)transform["rotation"][3];
		objectData.transform.rotation_ = Quaternion::QuaternionToEuler(quaternion);

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
			objectData.collider.colliderType = coll.at("type");
			// centerを取得
			Vector3 handle;
			handle.x = (float)coll.at("center").at(0);
			handle.y = (float)coll.at("center").at(2);
			handle.z = (float)coll.at("center").at(1);
			objectData.collider.center = handle;

			// sizeを取得
			handle.x = (float)coll.at("size").at(0);
			handle.y = (float)coll.at("size").at(2);
			handle.z = (float)coll.at("size").at(1);
			// こちらと合わせるために、1/2する
			objectData.collider.size = handle * 0.5f;
		}

	}
	// カメラだった場合
	else if (type.compare("CAMERA") == 0) {
		// 要素追加
		levelData->objects.emplace_back(LevelData::ObjectData());
		// 今追加した要素の参照を得る
		LevelData::ObjectData& objectData = levelData->objects.back();
		if (object.contains("name")) {
			// typenameの取得
			objectData.typeName = object["name"];
		}
		// トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];
		// 平行移動
		objectData.transform.translation_.x = (float)transform["translation"][0];
		objectData.transform.translation_.y = (float)transform["translation"][2];
		objectData.transform.translation_.z = (float)transform["translation"][1];

		// 回転角 Quaternionで取得し、Euler角に戻して渡す
		Quaternion quaternion;
		quaternion.x = -(float)transform["rotation"][0];
		quaternion.y = -(float)transform["rotation"][2];
		quaternion.z = -(float)transform["rotation"][1];
		quaternion.w = (float)transform["rotation"][3];
		objectData.transform.rotation_ = Quaternion::QuaternionToEuler(quaternion);
		// Blender側ではx0の時に下向き。ゲーム側ではx0の時に横向き(普通の視点)なのでx軸に+90度する
		objectData.transform.rotation_.x += AngleToRadian(90.0f);

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
	}

	// オブジェクト走査を再帰関数にまとめ、再帰呼び出しで枝を走査する
	if (object.contains("children")) {
		for (nlohmann::json& obj : object["children"]) {
			ObjectScan(levelData, obj, parent_ptr);
		}
	}
}
