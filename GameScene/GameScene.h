#pragma once
#include "Framework/GameFrame.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/BattleScene.h"
#include "Game/BoxSky/BoxSky.h"
#include <memory>
#include <optional>
#include "Transition.h"


class GameScene : public GameFrame {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:
	std::unique_ptr<TitleScene> titleScene_;
	std::unique_ptr<BattleScene> battleScene_;
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
	static void (GameScene::* SceneInitializeTable[])();
	static void (GameScene::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	void TitleInitialize();
	void BattleInitialize();
	void ResultInitialize();
	void TitleUpdate();
	void BattleUpdate();
	void ResultUpdate();

};
