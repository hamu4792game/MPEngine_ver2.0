#include "AnimationData.h"
#include "ResourceManager/ResourceManager.h"

AnimationData AnimationData::operator=(const AnimationData& animation) {
	this->duration = animation.duration;
	this->nodeAnimations = animation.nodeAnimations;
	return *this;
}

void AnimationData::Load(const std::string& name, const std::string& filename) {
	name_ = name;
	*this = ResourceManager::GetInstance()->LoadAnimationFile(filename);
}
