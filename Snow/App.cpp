#include "App.h"

#include "drawable/primitives/Box.h"
#include "drawable/Melon.h"
#include "drawable/Pyramid.h"

#include "SnMath.h"

// std
#include <memory>
#include <algorithm>

App::App()
	: _wnd(800, 600, L"Snow Engine") 
{
	class Factory
	{
	public:
		Factory(SnGraphics& gfx)
			: gfx(gfx) {}

		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		SnGraphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};
	
	Factory f(_wnd.Gfx());
	_drawables.reserve(_nDrawables);
	std::generate_n(std::back_inserter(_drawables), _nDrawables, f);

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
	
	for (auto& d : _drawables)
	{
		d->Update(dt);
		d->Draw(_wnd.Gfx());
	}

	_wnd.Gfx().EndFrame();
}

