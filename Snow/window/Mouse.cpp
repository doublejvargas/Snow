#include <Windows.h>
#include "Mouse.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { _x,_y };
}

int Mouse::GetPosX() const noexcept
{
	return _x;
}

int Mouse::GetPosY() const noexcept
{
	return _y;
}

bool Mouse::IsInWindow() const noexcept
{
	return _isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return _leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return _rightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (_buffer.size() > 0u)
	{
		Mouse::Event e = _buffer.front();
		_buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event(); // default constructor, basically an "invalid" event (see default constructor)
	}
}

void Mouse::Flush() noexcept
{
	_buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	_x = newx;
	_y = newy;

	_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	_isInWindow = false;
	_buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	_isInWindow = true;
	_buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	_leftIsPressed = true;

	_buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	_leftIsPressed = false;

	_buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	_rightIsPressed = true;

	_buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	_rightIsPressed = false;

	_buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	_wheelDeltaCarry += delta;
	// generate events for every 120
	while (_wheelDeltaCarry >= WHEEL_DELTA)
	{
		_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (_wheelDeltaCarry <= -WHEEL_DELTA)
	{
		_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::TrimBuffer() noexcept
{
	while (_buffer.size() > bufferSize)
	{
		_buffer.pop();
	}
}