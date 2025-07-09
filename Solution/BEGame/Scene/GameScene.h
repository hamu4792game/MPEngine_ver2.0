#pragma once
#include "Framework/SceneFrame.h"
#include "BEGame/Game/IStateFrame.h"
#include <memory>

class GameScene : public SceneFrame {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void SecondUpdate() override;

private:
	void ImGuiProc();
	void ChangeState(GameState state);
	void InitializeFactory() const;

public:
	std::unique_ptr<GameData> pdata_;
private:
	GameState state_;

	// ポインタ
	std::unique_ptr<IStateFrame> pStateUpdate_;

};