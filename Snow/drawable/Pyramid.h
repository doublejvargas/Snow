#pragma once
#include "DrawableBase.h"

class Pyramid : public DrawableBase<Pyramid>
{
public:
	Pyramid(SnGraphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	// positional
	float _r;
	float _roll = 0.0f;
	float _pitch = 0.0f;
	float _yaw = 0.0f;
	float _theta;
	float _phi;
	float _chi;
	// speed (delta/s)

	float _droll;
	float _dpitch;
	float _dyaw;
	float _dtheta;
	float _dphi;
	float _dchi;
};