#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(SnGraphics& gfx, const Drawable& parent)
	: _parent(parent)
{
	if (!_pVcbuf)
		_pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
}

void TransformCbuf::Bind(SnGraphics& gfx) noexcept
{
	_pVcbuf->Update(gfx,
		DirectX::XMMatrixTranspose(
			_parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	
	_pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::_pVcbuf;