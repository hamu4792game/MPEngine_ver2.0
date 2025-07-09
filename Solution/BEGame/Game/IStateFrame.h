#pragma once
#include <optional>

struct GameData {
	double time = 0.0;
};

enum GameState {
	WAIT_START, // 待機状態
	RUNNING, // 計測状態
	RESULT, // 結果表示
	RANKING // ランキング表示
};

class IStateFrame {
public:
	IStateFrame() = default;
	IStateFrame(GameData* data) : pdata_(data) {}
	virtual ~IStateFrame() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;
	
	std::optional<GameState> GetRequestedState() const {
		return requestedState_;
	}

protected:
	virtual void ImGuiProc() = 0;
	void RequestStateChange(GameState nextState) {
		requestedState_ = nextState;
	}

protected:
	GameData* pdata_ = nullptr;
	std::optional<GameState> requestedState_;
};