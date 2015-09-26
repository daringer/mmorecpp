#pragma once

// wrap different platforms to appropriate includes
#ifdef _WIN32
#include "tools_win.h"
#elif __unix__
#include "tools_lin.h"
#elif __linux__
#include "tools_lin.h"
#elif __APPLE__
#include "tools_lin.h"
#endif
