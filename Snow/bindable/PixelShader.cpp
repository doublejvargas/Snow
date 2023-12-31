#include "PixelShader.h"

// exception macros
#include "error/GraphicsThrowMacros.h"

PixelShader::PixelShader(SnGraphics& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &_pPixelShader));
}

void PixelShader::Bind(SnGraphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(_pPixelShader.Get(), nullptr, 0u);
}