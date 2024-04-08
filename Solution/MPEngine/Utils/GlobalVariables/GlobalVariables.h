#pragma once
#include "MPEngine/Math/MathUtl.h"
#include <variant>
#include <map>
#include <string>
#include <vector>

class GlobalVariables {
public:
	
	static GlobalVariables* GetInstance();

	void CreateGroup(const std::string& groupName);

	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, Vector2 value);
	void SetValue(const std::string& groupName, const std::string& key, Vector3 value);
	//void SetValue(const std::string& groupName, const std::string& key, Quaternion value);
	void SetValue(const std::string& groupName, const std::string& key, std::string value);

	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, const Vector2& value);
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);
	//void AddItem(const std::string& groupName, const std::string& key, const Quaternion& value);
	void AddItem(const std::string& groupName, const std::string& key, const std::string& value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector2 GetVector2Value(const std::string& groupName, const std::string& key) const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;
	//Quaternion GetQuaternionValue(const std::string& groupName, const std::string& key) const;
	std::string GetStringValue(const std::string& groupName, const std::string& key) const;

	bool HasValue(const std::string& groupName, const std::string& key) const;

	void SaveFile(const std::string& groupName);

	void LoadFiles();

	void LoadFile(const std::string& groupName);

	void ChackFiles(std::vector<std::string>& fileName);

	void LoadMessage(const std::string& groupName);
	void SaveMessage(const std::string& groupName);
	

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;

private:

	using Item = std::variant<int32_t, float, Vector2, Vector3, /*Quaternion,*/ std::string>;

	using Group = std::map<std::string, Item>;

	std::map<std::string, Group> datas_;

	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};

