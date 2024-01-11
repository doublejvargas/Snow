#pragma once
#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
	Box(SnGraphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	// positional
	// rotation about the box's center
	float _r;
	float _roll = 0.0f;
	float _pitch = 0.0f;
	float _yaw = 0.0f;
	// position of box
	float _theta;
	float _phi;
	float _chi;
	// speed (delta/s)
	// rotation about the world space's origin
	float _droll;
	float _dpitch;
	float _dyaw;
	// translation
	float _dtheta;
	float _dphi;
	float _dchi;
};