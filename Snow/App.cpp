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

	_wnd.Gfx().DrawTestTriangle(
		-_timer.Peek(),
		0.0f,
		0.0f
	);

	_wnd.Gfx().DrawTestTriangle(
		_timer.Peek(), 
		_wnd.mouse.GetPosX() / (_wnd.GetWidth() / 2.0f) - 1.0f,		// the manipulation here has to do with the difference in coordinate systems between pipeline and mouse
		-_wnd.mouse.GetPosY() / (_wnd.GetHeight() / 2.0f) + 1.0f    //    rendering: normalized device coordinates (-1, 1), y goes down; mouse: not normalized screen coordinates, y goes up
	);  
	_wnd.Gfx().EndFrame();
}

