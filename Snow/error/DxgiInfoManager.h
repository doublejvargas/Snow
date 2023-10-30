#pragma once

//std
#include <string>
#include <vector>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	unsigned long long _next = 0u;
	struct IDXGIInfoQueue* _pDxgiInfoQueue = nullptr;
};