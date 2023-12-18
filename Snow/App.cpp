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
	const float c = sin(_timer.Peek()) / 2.f + .5f;
	_wnd.Gfx().ClearBuffer(c, c, 1.f); //sin wave to animate ClearBuffer: oscillates from RGB white (1, 1, 1) to blue (0, 0, 1)
	_wnd.Gfx().DrawTestTriangle(_timer.Peek());
	_wnd.Gfx().EndFrame();
}

