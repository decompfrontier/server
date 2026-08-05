#ifdef __APPX__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "gamefrontend.h"

// MSVC requires __declspec(dllexport) BEFORE the return type, not after
// __stdcall.  The previous declaration order ("extern \"C\" void __stdcall
// __declspec(dllexport) OfflineMod_startup") produced
//   error C2059: syntax error: '__declspec(dllexport)'
// on the first real release-win32 build (the only preset that defines
// __APPX__ and reaches this file).  The debug-win64 preset doesn't define
// __APPX__, so the #ifdef'd block had never been compiled before.
extern "C" __declspec(dllexport) void __stdcall OfflineMod_startup(void)
{
    // TODO: populate
}

#endif
