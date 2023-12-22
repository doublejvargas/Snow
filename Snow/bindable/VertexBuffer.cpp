#include "VertexBuffer.h"

void VertexBuffer::Bind(SnGraphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, _pVertexBuffer.GetAddressOf(), &_stride, &offset);
}