#include <windows.h>

#include <ddraw.h>

#include "core/utils/error.h"
#include "core/utils/log.h"

extern "C"
{

void WINAPI AcquireDDThreadLock()
{
    gems::die("export should not be called directly");
}

void WINAPI ReleaseDDThreadLock()
{
    gems::die("export should not be called directly");
}

void WINAPI ReleaseDDTHreadLock()
{
    gems::die("export should not be called directly");
}

::HRESULT WINAPI DirectDrawEnumerateA(::LPDDENUMCALLBACKA, ::LPVOID)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawEnumerateW(::LPDDENUMCALLBACKW, ::LPVOID)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawEnumerateExA(::LPDDENUMCALLBACKEXA, ::LPVOID, ::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawEnumerateExW(::LPDDENUMCALLBACKEXW, ::LPVOID, ::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawCreateClipper(::DWORD, ::LPDIRECTDRAWCLIPPER *, ::IUnknown *)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawCreate(::GUID *, ::LPDIRECTDRAW *, ::IUnknown *)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DirectDrawCreateEx(::GUID *, ::LPVOID *, REFIID, ::IUnknown *)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI D3DParseUnknownCommand(::LPVOID, ::LPVOID *)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI GetSurfaceFromDC(::HDC, ::LPDIRECTDRAWSURFACE4 *, ::HDC *)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DllCanUnloadNow()
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DllGetClassObject(REFCLSID, REFIID, void **)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI CompleteCreateSysmemSurface()
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DDGetAttachedSurfaceLcl(::DWORD, ::DWORD, ::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DDInternalLock(::DWORD, ::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DDInternalUnlock(::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI DSoundHelp(::DWORD, ::DWORD, ::DWORD)
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI GetDDSurfaceLocal()
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI GetOLEThunkData()
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI RegisterSpecialCase()
{
    gems::die("export should not be called directly");
    return {};
}

::HRESULT WINAPI SetAppCompatData()
{
    gems::die("export should not be called directly");
    return {};
}

::BOOL WINAPI DllMain(::HMODULE module, ::DWORD reason, void *)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        ::DisableThreadLibraryCalls(module);
        gems::log("DllMain called for ddraw.dll");
    }
    return TRUE;
}
}
