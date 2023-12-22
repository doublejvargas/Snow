#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(SnGraphics& gfx, const std::wstring& path);
	void Bind(SnGraphics& gfx) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> _pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
};