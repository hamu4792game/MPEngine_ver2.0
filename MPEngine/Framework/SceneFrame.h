#pragma once

class SceneFrame {
public:
	SceneFrame() = default;
	virtual ~SceneFrame() = default;

	// 初期化
	virtual void Initialize() = 0;
	// 終了処理
	virtual void Finalize() = 0;
	// 更新処理
	virtual void Update() = 0;

	// 終了チェック
	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
};
