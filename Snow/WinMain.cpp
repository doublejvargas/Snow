#include <window/light_Windows.h>
#include "window/SnWindow.h"
#include "error/SnException.h"

//std
#include <sstream>

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
			
			// test code
			static int i = 0;
			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::WheelUp:
					i++;
					{
						std::ostringstream oss;
						oss << "Up: " << i;
						std::string s = oss.str();
						std::wstring ws(s.begin(), s.end());
						wnd.SetTitle(ws.c_str());
					}
					break;
				case Mouse::Event::Type::WheelDown:
					i--;
					{
						std::ostringstream oss;
						oss << "Down: " << i;
						std::string s = oss.str();
						std::wstring ws(s.begin(), s.end());
						wnd.SetTitle(ws.c_str());
					}
					break;
				}
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