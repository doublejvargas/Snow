#include "SnGraphics.h"

#pragma comment(lib, "d3d11.lib") // tell the linker which dll to link against

SnGraphics::SnGraphics(HWND hWnd)
{
	// Descriptor (very akin to vulkan!)
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;		// anti-aliasing
	sd.SampleDesc.Quality = 0;		// anti-aliasing
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;				// 1 front buffer & 1 back buffer
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(
		nullptr,						// video adapter, nullptr = default
		D3D_DRIVER_TYPE_HARDWARE,		// type of logical device: hardware, software, warp, etc..
		nullptr,						// handle to software driver if we wish to use
		0,								// flags
		nullptr,						// feature levels we wish to allow, default = anything
		0,								//
		D3D11_SDK_VERSION,				// sdk version we are targeting
		&sd,							// pointer to swap chain descriptor (swap chain configuration)
		&_pSwap,						// pointer to be filled w swap chain ("pp" -> pointer to pointer)
		&_pDevice,						// pointer to be filled w device
		nullptr,						// pointer to be filled w secured feature level
		&_pContext						// pointer to be filled w context
	);
	// gain access to texture subresource in swap chain (back buffer)
	ID3D11Resource* pBackBuffer = nullptr; // temp!
	_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	_pDevice->CreateRenderTargetView(
		pBackBuffer,		// resource or texture (inside swap chain)
		nullptr,			// descriptor for additional configs
		&_pTargetView		// pp that will be filled with handle to target view
	);
	pBackBuffer->Release();
}

SnGraphics::~SnGraphics()
{
	
	if (_pTargetView != nullptr)
	{
		_pTargetView->Release();
	}
	if (_pContext != nullptr)
	{
		_pContext->Release();
	}
	if (_pSwap != nullptr)
	{
		_pSwap->Release();
	}
	if (_pDevice != nullptr)
	{
		_pDevice->Release();
	}
}

void SnGraphics::EndFrame()
{
	_pSwap->Present(1u, 0u);
}

void SnGraphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	_pContext->ClearRenderTargetView(_pTargetView, color);
}

