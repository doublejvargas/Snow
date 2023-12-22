#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(SnGraphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(SnGraphics& gfx) noexcept override;

protected:
	D3D11_PRIMITIVE_TOPOLOGY _type;
};