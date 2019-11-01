// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifndef stdafx_h__
#define stdafx_h__

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

//Standard C++ headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

//Standard headers
#include <d3d9.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <array>
#include <string>

#define SWRS_USES_HASH

//SWRSToys headers
#include "fields.h"
#include "swrs.h"

// reference additional headers your program requires here

#endif // stdafx_h__
