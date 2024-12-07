#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <vector>
#include <array>

class GameUI {
public:
	GameUI() = default;
	~GameUI() = default;

	void Initialize(const uint32_t& collectionNum);
	void Update(const uint32_t& collectionNum);

private:
	void ImGuiProc();

private:
	enum UniqueHud {
		MoveKey,
		Mouse,
		Escape,

		kMaxNum
	};
	std::array<std::shared_ptr<Sprite>, UniqueHud::kMaxNum> huds_;

	std::vector<std::shared_ptr<Sprite>> collectionObjects_;


	std::array<std::string, UniqueHud::kMaxNum> textureNames{
		"MoveKey",
		"Mouse",
		"Escape"
	};
	struct Parameter {
		Vector2 scale;
		Vector2 translate;
		Vector2 uvScale;
		Vector2 uvTranslate;
		Parameter(Vector2 s, Vector2 t, Vector2 us, Vector2 ut) : scale(s), translate(t), uvScale(us), uvTranslate(ut) {}
	};
	std::array<Parameter, UniqueHud::kMaxNum> spriteParam{
		Parameter{Vector2(170.0f,100.0f),Vector2(-490.0f,-250.0f),Vector2(1.0f / 2.0f,1.0f),Vector2::zero},
		Parameter{Vector2(60.0f,90.0f),Vector2(490.0f,-250.0f),Vector2(1.0f / 2.0f,1.0f),Vector2::zero},
		Parameter{Vector2(50.0f,50.0f),Vector2(-555.0f,300.0f),Vector2::one,Vector2::zero},
	};

};
