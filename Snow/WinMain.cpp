#include <Windows.h>
#include <sstream>

// sample procedure defining the behavior of our snowWindow (swWindow)
LRESULT CALLBACK swWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(86); // the code that the application will return upon exit.
		}
		break;
	case WM_KEYUP:
	{
		if (wParam == 'F')
			SetWindowText(hWnd, L"F in the chat.");
	}
		break;
	case WM_CHAR:
	{
		static std::string title;
		title.push_back((char)wParam);
		auto temp = std::wstring(title.begin(), title.end());
		LPCWSTR s = temp.c_str();
		SetWindowText(hWnd, s);

	}
		break;
	case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			std::ostringstream oss;
			oss << "(" << pt.x << "," << pt.y << ")";
			std::string str = oss.str();
			auto temp = std::wstring(str.begin(), str.end());
			LPCWSTR s = temp.c_str();
			SetWindowText(hWnd, s);

		}
		break;
	}

	// default windows procedure after our own customization to provide default handling of all other events
	return DefWindowProc(hWnd, msg, wParam, lParam);	
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevIsntance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	const wchar_t pClassName[] = L"Snow Engine Window";
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;			// each window will have its own context, meaning they can be independently rendered to.
	wc.lpfnWndProc = swWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(
		CS_OWNDC,					// Optional window styles.
		pClassName,					// Window class
		L"Hello, Snow!",			// Window text
		WS_OVERLAPPEDWINDOW,		// Window style
		
		// size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,

		nullptr,					// Parent window    
		nullptr,					// Menu
		hInstance,					// Instance handle
		nullptr						// Additional application data
	);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	
	// event-based message loop to draw the window
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) // nullptr tells win32 api we want messages from the thread (i.e, all windows)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1)
		return -1;
	else
		return msg.wParam;
}