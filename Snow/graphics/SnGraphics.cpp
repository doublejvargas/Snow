#include "SnGraphics.h"

//lib
#include "vendor/dxerr/dxerr.h"
#include <d3dcompiler.h>

// std
#include <sstream>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib") // tell the linker which dll to link against for d3d stuff
#pragma comment(lib, "D3DCompiler.lib") // used to compile HLSL shaders at runtime, but we'll use it to access shader loading functions 

// graphics exception checking/throwing macros (some with DXGI infos)
#define GFX_EXCEPT_NOINFO(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if (FAILED(hr = (hrcall))) throw GFX_EXCEPT_NOINFO(hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) _infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) _infoManager.Set(); (call); {auto v = _infoManager.GetMessages(); if(!v.empty()) {throw SnGraphics::InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
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

void SnGraphics::DrawTestTriangle()
{
	HRESULT hr;
	namespace wrl = Microsoft::WRL;
	// struct that represents a vertex that only contains positions
	struct Vertex
	{
		float x;
		float y;
	};
	
	// vertex positions data "subresource data" (2d triangle at center of screen)
	const Vertex vertices[] = {
		{0.f,   .5f},
		{.5f,  -.5f},
		{-.5f, -.5f}

	};

	// COM object that represents our vertex buffer
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	// descriptor provides specifications for this buffer
	D3D11_BUFFER_DESC bd{};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	
	// another descriptor that intakes the vertex data (in this case, positions)
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices;
	
	// the device allocates the resources, so we use it to create the buffer on the GPU side
	GFX_THROW_INFO(_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer)); // don't forget to check for exceptions here
	
	// the context handles the operations/function calls that we will make on the graphics pipeline
	// here we bind the vertex buffer to the pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	_pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	// parameters: const void* pShaderByteCode, size_t byteCodeLength, id3d11classlinkage pClassLinkage, pp to be filled
	GFX_THROW_INFO(_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	
	// bind vertex shader
	_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// Issue draw calls from context pp
	GFX_THROW_INFO_ONLY(_pContext->Draw((UINT)std::size(vertices), 0u));
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

SnGraphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> info /*= {}*/) noexcept
	: Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : info)
	{
		_info += m;
		_info.push_back('\n');
	}
	// remove final newline if exists
	if (!_info.empty())
	{
		_info.pop_back();
	}
}

const char* SnGraphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	_whatBuffer = oss.str();
	return _whatBuffer.c_str();
}

const char* SnGraphics::InfoException::GetType() const noexcept
{
	return "SnGraphics Info Exception";
}

std::string SnGraphics::InfoException::GetErrorInfo() const noexcept
{
	return _info;
}
