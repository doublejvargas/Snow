#include <window/light_Windows.h>
#include "App.h"
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
		return App{}.Go();
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