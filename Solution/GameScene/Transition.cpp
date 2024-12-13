#include "Transition.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"

Transition::Transition() : fadeSpeed_(20.0f) {
	background_ = std::make_unique<Sprite>();
	background_->SetTexture(ResourceManager::GetInstance()->FindTexture("white2x2"));
	background_->SetLayerNum(10u);
	background_->SetIsActive(true);
	background_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

bool Transition::Update() {
	if (!isTransition_) {
		return false;
	}
	
	switch (type_) {
	case Transition::Type::Spin:
		if (ScalingSpinChange()) {
			return true;
		}
		break;
	case Transition::Type::BlackOut:
		if (FadingInOut()) {
			return true;
		}
		break;
	}

	
	return false;

}

void Transition::StartTransition(Type type) {
	if (isTransition_) { return; }
	isTransition_ = true;
	background_->SetIsActive(true);
	type_ = type;

	switch (type) {
	case Transition::Type::Spin:
		scale_ = Vector2::zero;
		color_ = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		background_->SetColor(color_);
		background_->SetScale(scale_);
		break;
	case Transition::Type::BlackOut:
		color_ = Vector4::zero;
		background_->SetColor(color_);
		background_->SetScale(Vector2(1280.0f, 720.0f));
		break;
	}

}

void Transition::EndProcess() {
	fadeInOutFlag_ = true;
	isTransition_ = false;
	background_->SetIsActive(false);
}

bool Transition::ScalingSpinChange() {
	const Vector2 kWindowsize = Vector2(1280.0f, 1280.0f);
	Vector2 volume = Vector2(kWindowsize.x / fadeSpeed_, kWindowsize.y / fadeSpeed_);
	float rotValue = 360.0f / fadeSpeed_;

	rotate_ += rotValue;

	if (fadeInOutFlag_) {
		scale_ += volume;
		if (scale_.x >= kWindowsize.x && scale_.y >= kWindowsize.y) {
			fadeInOutFlag_ = false;
			background_->SetScale(scale_);
			background_->SetRotate(AngleToRadian(rotate_));
			return true;
		}
	}
	else {
		scale_ -= volume;
		if (scale_.x <= 0.0f && scale_.y <= 0.0f) {
			EndProcess();
			rotate_ = 0.0f;
		}
	}
	
	background_->SetScale(scale_);
	background_->SetRotate(AngleToRadian(rotate_));
	return false;
}

bool Transition::FadingInOut() {
	float fadeSpeed = 0.1f;
	if (fadeInOutFlag_) {
		color_.w += fadeSpeed;
		if (color_.w >= 1.0f) {
			color_.w = 1.0f;
			fadeInOutFlag_ = false;
			background_->SetColor(color_);
			return true;
		}
	}
	else {
		color_.w -= fadeSpeed;
		if (color_.w <= 0.0f) {
			EndProcess();
		}
	}

	background_->SetColor(color_);
	return false;
}
