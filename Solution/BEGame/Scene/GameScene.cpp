#include "GameScene.h"
#include "ImGuiManager/ImGuiManager.h"
#include "BEGame/Game/WaitStart.h"
#include "BEGame/Game/Running.h"
#include "BEGame/Game/Result.h"
#include "BEGame/Game/Ranking.h"

#include <unordered_map>
#include <functional>
std::unordered_map<GameState, std::function<std::unique_ptr<IStateFrame>()>> stateFactoryMap;

void GameScene::Initialize() {
    // 待機状態でスタート
    pdata_ = std::make_unique<GameData>();
    pStateUpdate_ = std::make_unique<WaitStart>(pdata_.get());
    pStateUpdate_->Initialize();
    InitializeFactory();
}

void GameScene::Finalize() {
    if (pStateUpdate_) {
        pStateUpdate_->Finalize();
        pStateUpdate_.reset();
    }
    pdata_.reset();
}

void GameScene::Update() {
    // 早期リターン
    if (!pStateUpdate_) return;
    // 更新
    pStateUpdate_->Update();

    // 状態遷移
    if (auto nextState = pStateUpdate_->GetRequestedState()) {
        ChangeState(*nextState);
    }
}

void GameScene::SecondUpdate() {

}

void GameScene::ImGuiProc() {
#ifdef _DEBUG


#endif // _DEBUG
}

void GameScene::ChangeState(GameState state) {
    if (pStateUpdate_) {
        pStateUpdate_->Finalize();
    }

    auto it = stateFactoryMap.find(state);
    if (it != stateFactoryMap.end()) {
        pStateUpdate_ = it->second();
        pStateUpdate_->Initialize();
    }
}

void GameScene::InitializeFactory() const {
    GameData* data = pdata_.get(); // 生ポインタ取得

    stateFactoryMap = {
        { GameState::WAIT_START, [data] { return std::make_unique<WaitStart>(data); } },
        { GameState::RUNNING,    [data] { return std::make_unique<Running>(data); } },
        { GameState::RESULT,     [data] { return std::make_unique<Result>(data); } },
        { GameState::RANKING,    [data] { return std::make_unique<Ranking>(data); } }
    };
}
