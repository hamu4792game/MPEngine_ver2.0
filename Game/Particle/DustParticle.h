#pragma once
#include "Graphics/Particle/Particle.h"
#include <memory>
#include "Math/MathUtl.h"

class DustParticle {
public:
	DustParticle();
	~DustParticle() = default;

	void Initialize(const Vector3& emitter);
	void Update(const Vector3& emitter);
	void DrawImGui();
private:
	void StateInitialize(const Vector3& emitter, const uint32_t& num);

private:
	std::shared_ptr<Particle> particle_;
	const uint32_t kNumCount_ = 30u;
	struct Param {
		WorldTransform transform_;
		Vector3 velocity_;
		Vector4 color_ = Vector4::zero;
		bool isAlive_ = false;
		float speed_ = 0.3f;
	};
	std::vector<Param> particleparam_;
	Param minParam_;
	Param maxParam_;
	float coolTime_ = 0.0f;
	bool flag_ = false;

};