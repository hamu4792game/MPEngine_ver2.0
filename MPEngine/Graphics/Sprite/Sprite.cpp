#include "Sprite.h"
decltype(Sprite::spriteLists_) Sprite::spriteLists_;

Sprite::Sprite() {
	spriteLists_.emplace_back(this);
}

Sprite::~Sprite() {
	spriteLists_.remove(this);
}

