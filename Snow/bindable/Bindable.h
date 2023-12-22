#pragma once
#include "graphics/SnGraphics.h"

class Bindable
{
public:
	virtual void Bind(SnGraphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext* GetContext(SnGraphics& gfx) noexcept;
	static ID3D11Device* GetDevice(SnGraphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(SnGraphics& gfx) noexcept(!IS_DEBUG);
};