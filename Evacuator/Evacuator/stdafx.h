// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable:4091)
#include <imagehlp.h>
#pragma warning(pop)
#include <shlwapi.h>
#include <Shellapi.h>
#include <cstdio>
#include <boost/algorithm/string.hpp>
#include <tuple>
#include <string>
#include <fstream>
#include <vector>
