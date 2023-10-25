#include "SnWindow.h"

// icon
#include "resource.h"

// std
#include <sstream>

// declare static instance of the class here too?
SnWindow::SnWindowClass SnWindow::SnWindowClass::_snWndClass;

// Create and register the window class in the windows api
SnWindow::SnWindowClass::SnWindowClass() noexcept
	: _hInst{ GetModuleHandle(nullptr) }
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;				// each window will have its own context, meaning they can be independently rendered to.
	wc.lpfnWndProc = HandleMsgSetup;	// initially set up to this interface, but then this function registers SnWindow::HandleMsgThunk as the handler (which invokes HandleMsg()).
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(),
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 
		32, 32, 0
	));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(),
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		16, 16, 0
	));
	RegisterClassEx(&wc);
}

// Unregister the class from the windows api
SnWindow::SnWindowClass::~SnWindowClass()
{
	UnregisterClass(_wndClassName, GetInstance());
}

SnWindow::SnWindow(int width, int height, LPCWSTR name)
	: _width{width}, _height{height}
{
	// calculate the window size based on desired client region size (i.e., entire window including borders vs client region)
	RECT wr{};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (FAILED(AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE)))
	{
		throw SNHWND_LAST_EXCEPT();
	}
	// create window & get hWnd
	_hWnd = CreateWindowEx(
		CS_OWNDC,						// Optional window styles.
		SnWindowClass::GetName(),		// Window class
		name,							// Window text
		WS_OVERLAPPEDWINDOW,			// Window style

		// size and position
		CW_USEDEFAULT, CW_USEDEFAULT, (wr.right - wr.left), (wr.bottom - wr.top),

		nullptr,						// Parent window    
		nullptr,						// Menu
		SnWindowClass::GetInstance(),	// Instance handle
		this							// Additional application data !! very important !! we're passing a pointer to our egine's class type to the win api side!
	);

	if (_hWnd == nullptr)
	{
		throw SNHWND_LAST_EXCEPT();
	}
	// show window
	ShowWindow(_hWnd, SW_SHOWNORMAL);
}

SnWindow::~SnWindow()
{
	DestroyWindow(_hWnd);
}

LRESULT WINAPI SnWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindowEx() to store class pointer
	if (msg == WM_NCCREATE)
	{
		/* extract ptr to SnWindow class from CreateWindowEx() extra parameters(note that both ptr and object pointed to are constant, hence both "const")
			ref: the "this" parameter in the CreateWindowEx() call in the constructor. */
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		SnWindow* const pWnd = static_cast<SnWindow*>(pCreate->lpCreateParams);
		// tell winapi to store a pointer to an instance of SnWindow class at USERDATA of hWnd
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// register HandleMsgThunk as the message handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&SnWindow::HandleMsgThunk));
		// forward message to SnWindow class' handler (why?)
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI SnWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to SnWindow class
	/* think of this as retrieving the LPVOID data at GWLP_USERDATA (the parameter to which we passed "this" -- an instance of SnWindow -- associated with hWnd,
		then casting it to the appropriate type, that is a SnWindow. This is reinterpreting a 64bit void ptr to SnWindow ptr type. */
	SnWindow* const pWnd = reinterpret_cast<SnWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to SnWindow class' handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT SnWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		// We don't destroy window here as we have a destructor to do so, so we destroy window at App-termination time (i.e., when SnWindow 
		//  exits its scope in the app and is popped from stack), and not here. Otherwise, our destructor may give us an error by trying to destroy twice.
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// SnWindow inner exception class
SnWindow::Exception::Exception(int line, const char* file, HRESULT hr)
	: SnException(line, file), _hr{hr} {}

const char* SnWindow::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << "\n\n"
		<< GetOriginString();
	_whatBuffer = oss.str();
	return _whatBuffer.c_str();
}

const char* SnWindow::Exception::GetType() const noexcept
{
	return "SnWindow Exception";
}

std::string SnWindow::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr);
	
	if (nMsgLen == 0)
		return "Unidentified error code";
	
	std::string error = pMsgBuf;
	LocalFree(pMsgBuf);
	return error;
}

HRESULT SnWindow::Exception::GetErrorCode() const noexcept
{
	return _hr;
}

std::string SnWindow::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(_hr);
}
