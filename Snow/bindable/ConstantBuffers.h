#pragma once
#include "Bindable.h"
#include "error/GraphicsThrowMacros.h"

template<typename C> // template on the entire class
class ConstantBuffer : public Bindable
{
public:
	/// <summary>
	/// Allows constant buffers to be updated every frame
	/// </summary>
	/// <param name="gfx"></param>
	/// <param name="consts"></param>
	void Update(SnGraphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr; // structure that gets filled with the data once the resource (in this case _pConstantBuffer) has been locked
		GFX_THROW_INFO(GetContext(gfx)->Map( // map is a function in the id3d11 device context class interface
			_pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, &consts, sizeof(consts)); // copy data from the argument consts into the locked subresource using the pointer in msr.Pdata, which points to the subresource
		GetContext(gfx)->Unmap(_pConstantBuffer.Get(), 0u); // unmap after done
	}
	ConstantBuffer(SnGraphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &_pConstantBuffer));
	}
	ConstantBuffer(SnGraphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &_pConstantBuffer));
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C> // does template affect inheritance??
{
	/* These two declarations are necessary because it seems that
	*   with template inheritance, the compiler can't find _pconstantBuffer ( in parent) and function GetContext (in grandparent) in parent classes! 
	    alternatively, one could use "this->GetContext(gfx)... in the Bind function below. But these using statements are more clear
		as to what is happening in my opinion than the this->*/
	using ConstantBuffer<C>::_pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(SnGraphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, _pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::_pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(SnGraphics& gfx) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, _pConstantBuffer.GetAddressOf());
	}
};