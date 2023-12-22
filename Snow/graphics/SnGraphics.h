#pragma once

#include "window/light_Windows.h"
#include "error/SnException.h"
#include "error/DxgiInfoManager.h"

// lib
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory.h>
#include <random>

// std
#include <vector>

class SnGraphics
{

	// design: classes which implement Bindable interface will have LIMITED access to SnGraphics' private methods/fields via protected methods
	//   note: "friend" status is not inherited, so child classes only have access to what Bindable lets them have access to via protected methods.
	friend class Bindable;

public:
	class Exception : public SnException
	{
		using SnException::SnException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;			// overriding the std::exception method
		const char* GetType() const noexcept override;		// overriding the SnException::GetType() method
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT _hr;
		std::string _info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string _reason;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> info = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string _info;
	};

public:
	SnGraphics(HWND hWnd); 
	// delete copy and assignment constructor/operator
	SnGraphics(const SnGraphics&) = delete;
	SnGraphics& operator=(const SnGraphics&) = delete;
	~SnGraphics() = default;
	
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

private:
	DirectX::XMMATRIX _projection;

#ifndef NDEBUG
	DxgiInfoManager _infoManager{};
#endif
	// COM smart pointers will improve garbage collection over manual Release() calls
	Microsoft::WRL::ComPtr<ID3D11Device> _pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _pDSV;
};
