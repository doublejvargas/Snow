#pragma once

#include "window/light_Windows.h"
#include <d3d11.h>

class SnGraphics
{
public:
	SnGraphics(HWND hWnd); 
	// delete copy and assignment constructor/operator
	SnGraphics(const SnGraphics&) = delete;
	SnGraphics& operator=(const SnGraphics&) = delete;
	~SnGraphics();
	
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
	ID3D11Device* _pDevice = nullptr;
	IDXGISwapChain* _pSwap = nullptr;
	ID3D11DeviceContext* _pContext = nullptr;
	ID3D11RenderTargetView* _pTargetView = nullptr;
};
