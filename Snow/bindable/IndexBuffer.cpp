#include "IndexBuffer.h"

// exception macros
#include "error/GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer(SnGraphics& gfx, const std::vector<unsigned short>& indices)
	: _count((UINT)indices.size())
{
	// this macro creates a reference of the info manager member field of the SnGraphics object gfx that is passed as an argument.
	// the macro assumes that a function "GetInfoManager(SnGraphics gfx) exists. In this case it does, see Bindable.h
	INFOMAN(gfx);

	// index buffer descriptor/specification
	D3D11_BUFFER_DESC ibd{};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(_count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	
	// index buffer subresource data
	D3D11_SUBRESOURCE_DATA isd{};
	isd.pSysMem = indices.data();

	// we call the device to create resources on the gpu memory
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &_pIndexBuffer));
}

void IndexBuffer::Bind(SnGraphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return _count;
}