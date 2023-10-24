#include <window/light_Windows.h>
#include "window/SnWindow.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevIsntance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	sn::SnWindow wnd(800, 300, L"Hello, Snow!");
	sn::SnWindow wnd2(200, 650, L"Lollll");
	
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