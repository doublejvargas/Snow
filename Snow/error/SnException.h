#pragma once

// std
#include <exception>
#include <string>

class SnException : public std::exception
{
public:
	SnException(int line, const char* file) noexcept;

	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int _line;
	std::string _file;
protected:
	mutable std::string _whatBuffer; // marked as mutable because it needs to be changed by what() function which is const

};