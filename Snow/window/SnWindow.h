#pragma once

#include "window/light_Windows.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include "error/SnException.h"
#include "graphics/SnGraphics.h"

// std
#include <optional>
#include <memory>

// private singleton class, manages registration/cleanup of window class
class SnWindow
{
public:
	class Exception : public SnException
	{
	public:
		Exception(int line, const char* file, HRESULT hr);
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT _hr;
	};
private:
	class SnWindowClass
	{
	public:
		inline static LPCWSTR GetName() noexcept { return _wndClassName; }
		inline static HINSTANCE GetInstance() noexcept { return _snWndClass._hInst; }
	private:
		SnWindowClass() noexcept;
		~SnWindowClass();
			
		// Delete copy constructor & operator
		SnWindowClass(const SnWindowClass &) = delete;
		SnWindowClass &operator=(const SnWindowClass &) = delete;
	private:
		static constexpr LPCWSTR _wndClassName = L"Snow Engine Window";
		static SnWindowClass _snWndClass;
		HINSTANCE _hInst;
	};

public:
	SnWindow(int width, int height, LPCWSTR name);
	~SnWindow();

	// Delete copy constructor & operator
	SnWindow(const SnWindow &) = delete;
	SnWindow &operator=(const SnWindow &) = delete;

	void SetTitle(LPCWSTR title);
	static std::optional<int> ProcessMessages();

	SnGraphics& Gfx();
private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd{};
	Mouse mouse{};
private:
	int _width;
	int _height;
	HWND _hWnd{};
	std::unique_ptr<SnGraphics> _pGfx;
};

// error exception helper macro
#define SNHWND_EXCEPT(hr) SnWindow::Exception(__LINE__, __FILE__, hr)
#define SNHWND_LAST_EXCEPT() SnWindow::Exception(__LINE__, __FILE__, GetLastError())