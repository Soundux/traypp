#pragma once

#if defined(_WIN32)
#include "tray/windows/windows.hpp"
#elif defined(__linux__)
#include "tray/linux/appindicator.hpp"
#endif