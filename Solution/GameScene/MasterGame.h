#pragma once
#include "Framework/GameFrame.h"
#include "Framework/SceneFrame.h"
#include <memory>
#include <optional>
#include "Transition.h"


class MasterGame : public GameFrame {
public:
	MasterGame() = default;
	~MasterGame() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


private:
	std::unique_ptr<SceneFrame> titleScene_;
	std::unique_ptr<SceneFrame> battleScene_;
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
