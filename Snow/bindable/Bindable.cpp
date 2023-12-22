#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(SnGraphics& gfx) noexcept
{
	return gfx._pContext.Get();
}

ID3D11Device* Bindable::GetDevice(SnGraphics& gfx) noexcept
{
	return gfx._pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(SnGraphics& gfx) noexcept(!IS_DEBUG)
{
#ifndef NDEBUG
	return gfx._infoManager;
#else
	throw std::logic_error("You cannot access gfx.infoManager in Release config!");
#endif
}