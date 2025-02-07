#pragma once
#include "Framework/GameFrame.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/BattleScene.h"
#include <memory>
#include <optional>
#include "Transition.h"


class MasterGame : public MPE::GameFrame {
public:
	MasterGame() = default;
	~MasterGame() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


private:
	std::unique_ptr<MPE::SceneFrame> titleScene_;
	std::unique_ptr<MPE::SceneFrame> battleScene_;
	std::unique_ptr<Transition> transition_;

	//Scene
	enum class Scene {
		TITLE,
		BATTLE,
		RESULT,

		kSceneNum
	};

	Scene scene_ = Scene::TITLE;
	Scene nextScene = Scene::BATTLE;
	static void (MasterGame::* SceneInitializeTable[])();
	static void (MasterGame::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	void TitleInitialize();
	void BattleInitialize();
	void TitleUpdate();
	void BattleUpdate();

};
