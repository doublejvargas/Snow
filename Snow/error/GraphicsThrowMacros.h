#pragma once

// HRESULT hr should exist in the local scope for these macros to work

#define GFX_EXCEPT_NOINFO(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if (FAILED(hr = (hrcall))) throw GFX_EXCEPT_NOINFO(hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) SnGraphics::HrException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) _infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr), _infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) _infoManager.Set(); (call); {auto v = _infoManager.GetMessages(); if(!v.empty()) {throw SnGraphics::InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) SnGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing info manager into local scope
// this.GetInfoManager() must exist
#ifdef NDEBUG
#define INFOMAN() HRESULT hr
#else
#define INFOMAN() HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager()
#endif