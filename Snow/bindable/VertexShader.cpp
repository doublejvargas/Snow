#include "VertexShader.h"

// exception macros
#include "error/GraphicsThrowMacros.h"


VertexShader::VertexShader(SnGraphics& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &_pBytecodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		_pBytecodeBlob->GetBufferPointer(),
		_pBytecodeBlob->GetBufferSize(),
		nullptr,
		&_pVertexShader
	));
}

void VertexShader::Bind(SnGraphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(_pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return _pBytecodeBlob.Get();
}