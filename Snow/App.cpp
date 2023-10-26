#include "App.h"

App::App()
	: _wnd(800, 600, L"Snow Engine") 
{}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but do not block for new messages
		if (const auto ecode = SnWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		OnTick();
	}
}

void App::OnTick()
{
	const float t = _timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	std::string s = oss.str();
	std::wstring ws = std::wstring(s.begin(), s.end());
	_wnd.SetTitle(ws.c_str());
}

