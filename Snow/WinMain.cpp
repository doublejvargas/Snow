#include <window/light_Windows.h>
#include "window/SnWindow.h"
#include "error/SnException.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevIsntance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	try
	{
		SnWindow wnd(800, 300, L"Hello, Snow!");
		// event-based message loop to draw the window
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) // nullptr tells win32 api we want messages from the app threads (i.e, all windows)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (wnd.kbd.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, L"Something happen!", L"Space key was pressed", MB_OK | MB_ICONEXCLAMATION);
			}
		}

		if (gResult == -1)
			return -1;
		else
			return msg.wParam;
	}
	catch (const SnException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}