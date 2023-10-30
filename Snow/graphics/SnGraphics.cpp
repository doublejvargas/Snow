#include "SnGraphics.h"

//lib
#include "vendor/dxerr/dxerr.h"

// std
#include <sstream>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib") // tell the linker which dll to link against

// graphics exception checking/throwing macros (some with DXGI infos)
#define GFX_EXCEPT_NOINFO(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if (FAILED(hr = (hrcall))) throw GFX_EXCEPT_NOINFO(hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) _infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#endif


SnGraphics::SnGraphics(HWND hWnd)
{
	// Descriptor (very akin to Vulkan!)
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
	sd.BufferCount = 2;				// double buffering
	sd.OutputWindow = /*(HWND)696969*/hWnd;		// uncomment to test debug messages!
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	// create device and front/back buffers, and swap chain and rendering context
	// the GFX_THROW_FAILED macro requires a local HRESULT hr variable to be used, see the one above
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,						// video adapter, nullptr = default
		D3D_DRIVER_TYPE_HARDWARE,		// type of logical device: hardware, software, warp, etc..
		nullptr,						// handle to software driver if we wish to use
		swapCreateFlags,				// flags
		nullptr,						// feature levels we wish to allow, default = anything
		0,								//
		D3D11_SDK_VERSION,				// sdk version we are targeting
		&sd,							// pointer to swap chain descriptor (swap chain configuration)
		&_pSwap,						// pointer to be filled w swap chain ("pp" -> pointer to pointer)
		&_pDevice,						// pointer to be filled w device
		nullptr,						// pointer to be filled w secured feature level
		/* the & operator is overloaded for ComPtrs and it is equivalent to calling the ReleaseAndGetAddressOf() method
		meaning, that calling & on a ComPtr will release any references that the ptr may hold. Be careful!
		in order to get the address [address of ComPtr itself], use the GetAddressOf() method. Here, it makes sense for us to use & as
		we're passing these pointers to fill them in, so it is good to release their contents, if any, first. */
		&_pContext						// pointer to be filled w context
	));

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer; // temp!
	GFX_THROW_INFO(_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(_pDevice->CreateRenderTargetView(
		// Get() returns the underlying [raw] ptr address (not the smart ptr address)
		pBackBuffer.Get(),		// resource or texture (inside swap chain)
		nullptr,				// descriptor for additional configs
		&_pTargetView			// pp that will be filled with handle to target view
	));
	
	// when program exits the scope of this function, pBackBuffer is automatically deleted
}

void SnGraphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	_infoManager.Set();
#endif
	if (FAILED(hr = _pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(_pDevice->GetDeviceRemovedReason());
		else
			throw GFX_EXCEPT(hr);
	}
}

void SnGraphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	_pContext->ClearRenderTargetView(_pTargetView.Get(), color);
}

// Graphics exception stuff

SnGraphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), _hr{hr}
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		_info += m;
		_info.push_back('\n');
	}
	// remove final newline if it exists
	if (!_info.empty())
		_info.pop_back();
}

const char* SnGraphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!_info.empty()) {
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl;
	}
	oss << GetOriginString();
	_whatBuffer = oss.str();
	return _whatBuffer.c_str();
}

const char* SnGraphics::HrException::GetType() const noexcept
{
	return "SnGraphics Exception";
}

HRESULT SnGraphics::HrException::GetErrorCode() const noexcept
{
	return _hr;
}

std::string SnGraphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorStringA(_hr);
}

std::string SnGraphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescriptionA(_hr, buf, sizeof(buf));
	return buf;
}

std::string SnGraphics::HrException::GetErrorInfo() const noexcept
{
	return _info;
}

const char* SnGraphics::DeviceRemovedException::GetType() const noexcept
{
	return "Sn Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
