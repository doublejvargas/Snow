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

void SnGraphics::DrawTestTriangle(float angle)
{
	HRESULT hr;
	namespace wrl = Microsoft::WRL;
	// struct that represents a vertex that only contains positions
	struct Vertex
	{
		struct  
		{
			float x;
			float y;
		} pos;

		struct  
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};
	
	// vertex positions data "subresource data" (2d triangle at center of screen)
	const Vertex vertices[] = {
		{0.f,   .5f, 255,     0,     0, 0},
		{.5f,  -.5f,   0,	255,     0, 0},
		{-.5f, -.5f,   0,     0,   255, 0},
		{-.3f,  .3f,   0,   255,     0, 0},
		{0.3f, 0.3f,   0,     0,   255, 0},
		{0.0f, -.8f, 255,     0,   255, 0},

	};

	// COM object that represents our vertex buffer
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	// descriptor provides specifications for this buffer
	D3D11_BUFFER_DESC vbd{};
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(vertices);
	vbd.StructureByteStride = sizeof(Vertex);
	
	// another descriptor that intakes the vertex data
	D3D11_SUBRESOURCE_DATA vsd{};
	vsd.pSysMem = vertices;
	
	// the device allocates the resources, so we use it to create the buffer on the GPU side
	GFX_THROW_INFO(_pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer)); // don't forget to check for exceptions here

	// the context handles the operations/function calls that we will make on the graphics pipeline
	// here we bind the vertex buffer to the pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// create index buffer
	const unsigned short indices[] =  // 16bit data structure
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5
	};

	// COM object that represents our index buffer
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	// descriptor provides specifications for this buffer
	D3D11_BUFFER_DESC ibd{};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	
	// another descriptor that intakes the index data
	D3D11_SUBRESOURCE_DATA isd{};
	isd.pSysMem = indices;

	// the device allocates the resources, so we use it to create the buffer on the GPU side
	GFX_THROW_INFO(_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer)); // don't forget to check for exceptions here

	// bind index buffer
	_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		struct  
		{
			float element[4][4];
		} transformation;
	};

	const ConstantBuffer cb =
	{
		{
			std::cos(angle),   std::sin(angle), .0f,   .0f,
		   -std::sin(angle),   std::cos(angle), .0f,   .0f,
			.0f,			   .0f,				1.f,   .0f,
			.0f,			   .0f,				.0f,   1.f
		}
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd{};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	// this buffer is not an array as a vertex buffer is an array, so byte stride of "elements" doesn't apply here as it only contains 1 element.
	cbd.StructureByteStride = 0u;
	// the subresource data in the cpu (double float array 4x4 "matrix" in this case) to be passed to gpu memory
	D3D11_SUBRESOURCE_DATA csd{};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(_pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer)); // the & operator for first 2 arguments ordinarily return references,
	                                                                       //   the & operator for the 3rd argument is overloaded (COM object) and releases before returning
																		   //   it's reference.
	
	// bind constant buffer to vertex shader
	_pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	// parameters: const void* pShaderByteCode, size_t byteCodeLength, id3d11classlinkage pClassLinkage, pp to be filled
	GFX_THROW_INFO(_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// bind pixel shader
	_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)); //note: this call &pBlob releases whatever blob is pointing to and fills it with something new.
	GFX_THROW_INFO(_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	
	// bind vertex shader
	_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout (2d positions only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		/*semanticName: must match with the element name specified in vertex shader
		  semanticIndex: the index (i.e., layout location) for the element, also must match with shader (TODO: REVIEW)
		  format: specifies the format of the data, for vertex positions this would be 2 32-bit floats (r32g32)
		  input slot: "always 0" according to chili
		  AlignedByteOffset: the offset from the beginning of vertex structure data to this specific element
		  InputSlotClass: vertex vs index? for now we'll use vertex
		  InstanceDataStepRate: we're not working with instances yet, so 0*/
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	GFX_THROW_INFO(_pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(), // this function requires the shader bytecode to compare that the semantics specified in the layout match up
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// bind input layout
	_pContext->IASetInputLayout(pInputLayout.Get());

	// bind render target
	_pContext->OMSetRenderTargets(1u, _pTargetView.GetAddressOf(), nullptr); // NOTE: we use GetAddressOf method here instead of & operator because we do not want to FREE contents of underlying raw pointer

	// Set primitive topology to triangle list (groups of 3 vertices)
	_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	// we don't need to create a COM d3d11 object, we can simply call the function on the pipeline with the structure above
	// RS "rasterizer stage"
	_pContext->RSSetViewports(1u, &vp);

	// Issue draw calls from context pp
	GFX_THROW_INFO_ONLY(_pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
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
