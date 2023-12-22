#pragma once
#include "Bindable.h"


// exception macros
#include "error/GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V> // template on the constructor only
	VertexBuffer(SnGraphics& gfx, const std::vector<V>& vertices)
		: _stride(sizeof(V))
	{
		INFOMAN(gfx);
		
		// vertex buffer descriptor/specification
		D3D11_BUFFER_DESC bd{};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		bd.StructureByteStride = sizeof(V);
		
		// vertex buffer subresource data
		D3D11_SUBRESOURCE_DATA sd{};
		sd.pSysMem = vertices.data();
		
		// use device to create resources
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &_pVertexBuffer));
	}
	void Bind(SnGraphics& gfx) noexcept override;

protected:
	UINT _stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;
};