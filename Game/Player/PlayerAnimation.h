#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <vector>

class PlayerAnimation {
public:
	PlayerAnimation() = default;
	~PlayerAnimation() = default;
public:
	void SetPartsPtr(std::vector<WorldTransform> parts);


private:
	std::vector<WorldTransform> partsTrans_;

};