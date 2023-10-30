#pragma once

#include "window/light_Windows.h"
#include "error/SnException.h"
#include "error/DxgiInfoManager.h"

// lib
#include <d3d11.h>

// std
#include <vector>

class SnGraphics
{
public:
	class Exception : public SnException
	{
		using SnException::SnException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
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

public:
	SnGraphics(HWND hWnd); 
	// delete copy and assignment constructor/operator
	SnGraphics(const SnGraphics&) = delete;
	SnGraphics& operator=(const SnGraphics&) = delete;
	~SnGraphics();
	
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
#ifndef NDEBUG
	DxgiInfoManager _infoManager{};
#endif
	ID3D11Device* _pDevice = nullptr;
	IDXGISwapChain* _pSwap = nullptr;
	ID3D11DeviceContext* _pContext = nullptr;
	ID3D11RenderTargetView* _pTargetView = nullptr;
};
