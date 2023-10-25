#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return _keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (_keyBuffer.size() > 0u)
	{
		Keyboard::Event e = _keyBuffer.front();
		_keyBuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return _keyBuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (_charBuffer.size() > 0u)
	{
		unsigned char charcode = _charBuffer.front();
		_charBuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return _charBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	_keyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	_charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	_autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	_autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return _autorepeatEnabled;
}

// methods used by SnWindow class
void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	_keyStates[keycode] = true;
	_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::PRESS, keycode));
	TrimBuffer(_keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	_keyStates[keycode] = false;
	_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::RELEASE, keycode));
	TrimBuffer(_keyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	_charBuffer.push(character);
	TrimBuffer(_charBuffer);
}

void Keyboard::ClearState() noexcept
{
	_keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
