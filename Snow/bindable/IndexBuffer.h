#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(SnGraphics& gfx, const std::vector<unsigned short>& indices);
	void Bind(SnGraphics& gfx) noexcept override;
	UINT GetCount() const noexcept;

protected:
	UINT _count;
	// needs to keep a pointer/reference to the index buffer because the function
	//   Drawable::Draw which calls SnGraphics::DrawIndexed which needs the index count to draw geometry.
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;
};