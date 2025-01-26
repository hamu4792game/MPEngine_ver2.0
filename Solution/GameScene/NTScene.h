#pragma once
#include "Framework/GameFrame.h"
#include <memory>
#include "Framework/SceneFrame.h"
#include "Transition.h"

class NTScene : public GameFrame {
public:
	NTScene() = default;
	~NTScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


private:

	enum Scene {
		TITLE,
		GAME,

		kMaxNum
	};
	Scene scene_ = TITLE;

	std::unique_ptr<SceneFrame> titleScene_;
	std::unique_ptr<SceneFrame> gameScene_;

	std::unique_ptr<Transition> transition_;
	
	static void (NTScene::* SceneInitializeTable[])();
	static void (NTScene::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	void TitleInitialize();
	void GameInitialize();
	void TitleUpdate();
	void GameUpdate();

};
