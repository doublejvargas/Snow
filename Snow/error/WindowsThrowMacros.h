#pragma once

#define SNHWND_EXCEPT(hr) SnWindow::HrException(__LINE__, __FILE__, hr)
#define SNHWND_LAST_EXCEPT() SnWindow::HrException(__LINE__, __FILE__, GetLastError())
#define SNHWND_NOGFX_EXCEPT() SnWindow::NoGfxException(__LINE__, __FILE__)