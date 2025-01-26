#pragma once
#include "MPEngine/Framework/SceneFrame.h"
#include <array>
#include <memory>
#include "NT/BaseNetwork.h"
#include "Graphics/Sprite/Sprite.h"

class GameScene : public SceneFrame {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
private:
	// アニメーション等の事後更新用
	void SecondUpdate() override;

	void SetNumber();

private:

	std::unique_ptr<BaseNetwork> network_;
	bool isServer_ = false;
	
	class Player {
	public:
		Player();
		int count_ = 30;
		bool isFinished_ = false;
		bool GetFinish() const { return isFinished_; }
		void Update();
		std::unique_ptr<Sprite> texture_;

	};
	std::array<std::unique_ptr<Player>, 2u> players_;
	// 数字
	std::array<std::unique_ptr<Sprite>, 4u> numbers_;

};