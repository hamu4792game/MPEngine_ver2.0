#pragma once
#include "MPEngine/Utils/WorldTransform/WorldTransform.h"

namespace MPE {
	// Cameraフレーム用
	class CameraFrame {
	public:
		CameraFrame() = default;
		virtual ~CameraFrame() = default;

		// 初期化
		virtual void Initialize(const WorldTransform& transform = WorldTransform()) {};
		// 終了処理
		virtual void Finalize() {};
		// 更新処理
		virtual void Update() {};

		// ゲッター
		const WorldTransform& GetTransform() { return transform_; }

	protected:
		// 初期化補間関数 カメラを切り替えた際に使用する
		WorldTransform InitializeInterpolationFunction() {
			return transform_;
		}

	protected:
		// メイン座標変数
		WorldTransform transform_;

	};

}