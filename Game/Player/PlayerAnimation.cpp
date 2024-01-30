#include "PlayerAnimation.h"
#include "Player.h"


void PlayerAnimation::SetPartsPtr(std::vector<WorldTransform> parts) {
	partsTrans_ = parts;
}
