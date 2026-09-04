#include "ddraw_export.h"

#include <windows.h>

#include <ddraw.h>

#include "core/utils/error.h"

namespace gems
{

struct DDRAW_EXPORT DDrawProxy
{
    void AcquireDDThreadLock()
    {
        gems::die("export should not be called directly AcquireDDThreadLock");
    }

    void ReleaseDDThreadLock()
    {
        gems::die("export should not be called directly ReleaseDDThreadLock");
    }

    void ReleaseDDTHreadLock()
    {
        gems::die("export should not be called directly ReleaseDDTHreadLock");
    }

    ::HRESULT DirectDrawEnumerateA(::LPDDENUMCALLBACKA, ::LPVOID)
    {
        gems::die("export should not be called directly DirectDrawEnumerateA");
        return {};
    }

    ::HRESULT DirectDrawEnumerateW(::LPDDENUMCALLBACKW, ::LPVOID)
    {
        gems::die("export should not be called directly DirectDrawEnumerateW");
        return {};
    }

    ::HRESULT DirectDrawEnumerateExA(::LPDDENUMCALLBACKEXA, ::LPVOID, ::DWORD)
    {
        gems::die("export should not be called directly DirectDrawEnumerateExA");
        return {};
    }

    ::HRESULT DirectDrawEnumerateExW(::LPDDENUMCALLBACKEXW, ::LPVOID, ::DWORD)
    {
        gems::die("export should not be called directly DirectDrawEnumerateExW");
        return {};
    }

    ::HRESULT DirectDrawCreateClipper(::DWORD, ::LPDIRECTDRAWCLIPPER *, ::IUnknown *)
    {
        gems::die("export should not be called directly DirectDrawCreateClipper");
        return {};
    }

    ::HRESULT DirectDrawCreate(::GUID *, ::LPDIRECTDRAW *, ::IUnknown *)
    {
        gems::die("export should not be called directly DirectDrawCreate");
        return {};
    }

    ::HRESULT DirectDrawCreateEx(::GUID *, ::LPVOID *, REFIID, ::IUnknown *)
    {
        gems::die("export should not be called directly DirectDrawCreateEx");
        return {};
    }

    ::HRESULT D3DParseUnknownCommand(::LPVOID, ::LPVOID *)
    {
        gems::die("export should not be called directly D3DParseUnknownCommand");
        return {};
    }

    ::HRESULT GetSurfaceFromDC(::HDC, ::LPDIRECTDRAWSURFACE4 *, ::HDC *)
    {
        gems::die("export should not be called directly GetSurfaceFromDC");
        return {};
    }

    ::HRESULT DllCanUnloadNow()
    {
        gems::die("export should not be called directly DllCanUnloadNow");
        return {};
    }

    ::HRESULT DllGetClassObject(REFCLSID, REFIID, void **)
    {
        gems::die("export should not be called directly DllGetClassObject");
        return {};
    }

    ::HRESULT CompleteCreateSysmemSurface()
    {
        gems::die("export should not be called directly CompleteCreateSysmemSurface");
        return {};
    }

    ::HRESULT DDGetAttachedSurfaceLcl(::DWORD, ::DWORD, ::DWORD)
    {
        gems::die("export should not be called directly DDGetAttachedSurfaceLcl");
        return {};
    }

    ::HRESULT DDInternalLock(::DWORD, ::DWORD)
    {
        gems::die("export should not be called directly DDInternalLock");
        return {};
    }

    ::HRESULT DDInternalUnlock(::DWORD)
    {
        gems::die("export should not be called directly DDInternalUnlock");
        return {};
    }

    ::HRESULT DSoundHelp(::DWORD, ::DWORD, ::DWORD)
    {
        gems::die("export should not be called directly DSoundHelp");
        return {};
    }

    ::HRESULT GetDDSurfaceLocal()
    {
        gems::die("export should not be called directly GetDDSurfaceLocal");
        return {};
    }

    ::HRESULT GetOLEThunkData()
    {
        gems::die("export should not be called directly GetOLEThunkData");
        return {};
    }

    ::HRESULT RegisterSpecialCase()
    {
        gems::die("export should not be called directly RegisterSpecialCase");
        return {};
    }

    ::HRESULT SetAppCompatData()
    {
        gems::die("export should not be called directly SetAppCompatData");
        return {};
    }
};

}

#include "core/dll_main/ddraw/ddraw_main.h"
