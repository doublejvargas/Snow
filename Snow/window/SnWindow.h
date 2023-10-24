#pragma once

#include "window/light_Windows.h"

namespace sn
{
	// private singleton class, manages registration/cleanup of window class
	class SnWindow
	{
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
		SnWindow(int width, int height, LPCWSTR name) noexcept;
		~SnWindow();

		// Delete copy constructor & operator
		SnWindow(const SnWindow &) = delete;
		SnWindow &operator=(const SnWindow &) = delete;
	private:
		static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	private:
		int _width;
		int _height;
		HWND _hWnd;
	};
} // namespace sn