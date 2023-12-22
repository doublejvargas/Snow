#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(SnGraphics& gfx, const std::wstring& path);
	void Bind(SnGraphics& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
};