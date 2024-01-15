#include "App.h"
#include "drawable/primitives/Box.h"

// std
#include <memory>

App::App()
	: _wnd(800, 600, L"Snow Engine") 
{
	// random
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 1.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	
	for (auto i = 0; i < 180; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			_wnd.Gfx(), rng, adist,
			ddist, odist, rdist
		));
	}
	_wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
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
	auto dt = _timer.Mark();
	
	_wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(_wnd.Gfx());
	}

	_wnd.Gfx().EndFrame();
}

