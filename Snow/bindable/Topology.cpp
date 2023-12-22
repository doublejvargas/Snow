#include "Topology.h"

Topology::Topology(SnGraphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	: _type(type)
{}

void Topology::Bind(SnGraphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(_type);
}