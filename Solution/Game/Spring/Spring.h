#pragma once
#include "Math/MathUtl.h"
#include "Graphics/Line/Line.h"
#include "Graphics/Model/Model.h"

class Spring {
public:
	Spring() = default;
	~Spring() = default;

	void Initialize();

	void Update();

private:
	void ImGuiProcess();

	void Move();

private:

	Vector3 anchor; // アンカー、固定された橋の位置
	float naturalLength = 0.0f; // 自然長
	float stiffness = 0.0f; // 剛性、ばね定数k
	float dampingCoefficient = 0.0f; // 減衰係数

	struct Ball {
		Vector3 position; // ボールの位置
		Vector3 velocity; // ボールの速度
		Vector3 acceleration; // ボールの加速度
		float mass = 0.0f; // ボールの質量
		float radius = 0.0f; // ボールの半径
	};
	Ball ball;

	std::unique_ptr<Line> springLine_;
	std::unique_ptr<Model> ballModel_;

};