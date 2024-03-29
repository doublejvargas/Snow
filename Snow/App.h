#pragma once

#include "Timer.h"
#include "window/SnWindow.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();

	~App();
private:
	void OnTick();

	SnWindow _wnd;
	Timer _timer{};
	std::vector<std::unique_ptr<class Drawable>> _drawables;
	static constexpr size_t _nDrawables = 180;
};