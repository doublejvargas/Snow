#include "InputLayout.h"

// exception macros
#include "error/GraphicsThrowMacros.h"

InputLayout::InputLayout(SnGraphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderBytecode)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		layout.data(), (UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&_pInputLayout
	));
}

void InputLayout::Bind(SnGraphics& gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(_pInputLayout.Get());
}