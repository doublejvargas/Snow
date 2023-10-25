#include "SnException.h"

// std
#include <sstream>

SnException::SnException(int line, const char* file) noexcept
	: _line{ line }, _file{ file } {}

const char* SnException::what() const noexcept
{
	std::ostringstream oss{};
	oss << GetType() << std::endl
		<< GetOriginString();
	_whatBuffer = oss.str();
	return _whatBuffer.c_str();
}

const char* SnException::GetType() const noexcept
{
	return "SnException";
}

int SnException::GetLine() const noexcept
{
	return _line;
}

const std::string& SnException::GetFile() const noexcept
{
	return _file;
}

std::string SnException::GetOriginString() const noexcept
{
	std::ostringstream oss{};
	oss << "[File] " << _file << std::endl
		<< "[Line] " << _line;
	return oss.str();
}
