#pragma once

// std
#include <bitset>
#include <queue>

// This class is an extension of the window class architecturally and is an interface between the window and game logic for handling of keyboard events.
class Keyboard
{
	friend class SnWindow;
public:
	class Event
	{
	public:
		enum class Type
		{
			PRESS,
			RELEASE,
			INVALID
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			: type{ Type::INVALID }, code{ 0u } {}

		Event(Type type, unsigned char code) noexcept
			: type{ type }, code{ code } {}

		inline bool IsPress() const noexcept			{ return type == Type::PRESS; }
		inline bool IsRelease() const noexcept			{ return type == Type::RELEASE; }
		inline bool IsValid() const noexcept			{ return type != Type::INVALID; }
		inline unsigned char GetCode() const noexcept	{ return code; }
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// key event stuff
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// char event stuff
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
	//These methods are used by SnWindow class (SnWindow is declared as a friend class), and are not visible to public user of class.
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool _autorepeatEnabled = false;
	std::bitset<nKeys> _keyStates;
	std::queue<Event> _keyBuffer;
	std::queue<char> _charBuffer;
};