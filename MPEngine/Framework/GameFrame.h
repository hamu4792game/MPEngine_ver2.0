#pragma once

class GameFrame {
public:
	GameFrame() = default;
	virtual ~GameFrame() = default;

	// 初期化
	virtual void Initialize() {};
	// 終了処理
	virtual void Finalize() {};
	// 更新処理
	virtual void Update() {};

	// 終了チェック
	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
};
