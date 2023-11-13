#include "GlobalVariables.h"
#include "externals/nlohmann/json.hpp"
#include "externals/imgui/imgui.h"
#include <fstream>
//#include <WinUser.h>
#include <Windows.h>
#undef max
#undef min

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables globalVariables;
	return &globalVariables;
}

void GlobalVariables::Update() {
#ifdef _DEBUG
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {

		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();
	     itGroup != datas_.end(); ++itGroup) {
		
		const std::string& groupName = itGroup->first;

		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		
		for (std::map<std::string, Item>::iterator itItem = group.begin();
		     itItem != group.end(); ++itItem) {
			
			const std::string& itemName = itItem->first;

			Item& item = itItem->second;


			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::DragInt(itemName.c_str(), ptr, 1);
			} else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.01f);
			} else if (std::holds_alternative<Vector2>(item)) {
				Vector2* ptr = std::get_if<Vector2>(&item);
				ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
			}
		}


		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}
		if (ImGui::Button("Load")) {
			LoadFile(groupName);
			std::string message = std::format("{}.json load", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}



	ImGui::EndMenuBar();
	ImGui::End();
#endif // _DEBUG
}

void GlobalVariables::CreateGroup(const std::string& groupName) { datas_[groupName]; }

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	
	Group& group = datas_[groupName];

	Item newItem{};
	newItem = value;

	group[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];

	Item newItem{};
	newItem = value;

	group[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, Vector2 value) {
	Group& group = datas_[groupName];

	Item newItem{};
	newItem = value;

	group[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, Vector3 value) {
	Group& group = datas_[groupName];

	Item newItem{};
	newItem = value;

	group[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {

	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	Group& group = itGroup->second;

	if (group.find(key) == group.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	Group& group = itGroup->second;

	if (group.find(key) == group.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector2& value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	Group& group = itGroup->second;

	if (group.find(key) == group.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	Group& group = itGroup->second;

	if (group.find(key) == group.end()) {
		SetValue(groupName, key, value);
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {

	assert(datas_.find(groupName) != datas_.end());

	const Group& group = datas_.at(groupName);

	assert(group.find(key) != group.end());

	return std::get<int32_t>(group.find(key)->second);
}

float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) const {

	assert(datas_.find(groupName) != datas_.end());

	const Group& group = datas_.at(groupName);

	assert(group.find(key) != group.end());

	return std::get<float>(group.find(key)->second);
}

Vector2 GlobalVariables::GetVector2Value(const std::string& groupName, const std::string& key) const {

	assert(datas_.find(groupName) != datas_.end());

	const Group& group = datas_.at(groupName);

	assert(group.find(key) != group.end());

	return std::get<Vector2>(group.find(key)->second);
}

Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {

	assert(datas_.find(groupName) != datas_.end());

	const Group& group = datas_.at(groupName);

	assert(group.find(key) != group.end());

	return std::get<Vector3>(group.find(key)->second);
}

void GlobalVariables::SaveFile(const std::string& groupName) {


	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	assert(itGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();
	
	
	root[groupName] = nlohmann::json::object();


	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin();
	     itItem != itGroup->second.end(); ++itItem) {
		
		const std::string& itemName = itItem->first;

		Item& item = itItem->second;


		if (std::holds_alternative<int32_t>(item)) {
			
			
			root[groupName][itemName] = std::get<int32_t>(item);
		} else if (std::holds_alternative<float>(item)) {

			root[groupName][itemName] = std::get<float>(item);
		} else if (std::holds_alternative<Vector2>(item)) {

			Vector2 value = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({value.x, value.y});
		}else if (std::holds_alternative<Vector3>(item)) {

			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z });
		}

	}

	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}


	std::string filePath = kDirectoryPath + groupName + ".json";

	std::ofstream ofs;

	ofs.open(filePath);

	if (ofs.fail()) {
		std::string message = "Failed open data file file for write";
#ifdef _DEBUG
		//MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // _DEBUG
		assert(0);
		return;
	}

	ofs << std::setw(4) << root << std::endl;

	ofs.close();
}

void GlobalVariables::LoadFiles() {

	std::filesystem::path dir(kDirectoryPath);

	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(dir);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		
		const std::filesystem::path& filePath = entry.path();


		std::string extension = filePath.extension().string();

		if (extension.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());
	}

}

void GlobalVariables::LoadFile(const std::string& groupName) {

	std::string filePath = kDirectoryPath + groupName + ".json";

	std::ifstream ifs;

	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "Failed open data file file for read";
#ifdef _DEBUG
		//MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // _DEBUG
		assert(0);
		return;
	}


	nlohmann::json root;

	ifs >> root;

	ifs.close();


	nlohmann::json::iterator itGroup = root.find(groupName);

	assert(itGroup != root.end());


	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		
		const std::string& itemName = itItem.key();


		if (itItem->is_number_integer()) {
			
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		} else if (itItem->is_number_float()) {

			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		} else if (itItem->is_array() && itItem->size() == 2) {

			Vector2 value = {itItem->at(0), itItem->at(1)};
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 3) {

			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
	}
}
