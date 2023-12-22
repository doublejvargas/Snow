#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(SnGraphics& gfx, const Drawable& parent)
	: _vcbuf(gfx), _parent(parent)
{}

void TransformCbuf::Bind(SnGraphics& gfx) noexcept
{
	_vcbuf.Update(gfx,
		DirectX::XMMatrixTranspose(
			_parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	
	_vcbuf.Bind(gfx);
}