#include <cstdint>
#include <cstring>

#define INITGUID
#include <windows.h>

#include <ddraw.h>

#include "core/utils/annotations.h"
#include "core/utils/com_hooking.h"
#include "core/utils/error.h"
#include "core/utils/formatter.h"
#include "core/utils/log.h"
#include "ddraw_export.h"

namespace gems
{

namespace IDirectDraw
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    [[maybe_unused]] ::HRESULT(WINAPI *orig)(::IDirectDraw *, ::HWND hwnd, ::DWORD dwFlags),
    ::IDirectDraw *that,
    ::HWND hwnd,
    ::DWORD dwFlags)
{
    log("IDirectDraw::SetCooperativeLevel({} {} {})",
        static_cast<void *>(that),
        hwnd,
        SetCooperativeLevelFlags{dwFlags});

    const auto res = orig(that, hwnd, dwFlags);
    log("IDirectDraw::SetCooperativeLevel res: {}", res);

    return res;
}

}

namespace IDirectDraw4
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    [[maybe_unused]] ::HRESULT(WINAPI *orig)(::IDirectDraw4 *, ::HWND hwnd, ::DWORD dwFlags),
    ::IDirectDraw4 *that,
    ::HWND hwnd,
    ::DWORD dwFlags)
{
    log("IDirectDraw4::SetCooperativeLevel({} {} {})",
        static_cast<void *>(that),
        hwnd,
        SetCooperativeLevelFlags{dwFlags});

    const auto res = orig(that, hwnd, dwFlags);
    log("IDirectDraw4::SetCooperativeLevel res: {}", res);

    return res;
}

}

namespace IDirectDraw7
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    [[maybe_unused]] ::HRESULT(WINAPI *orig)(::IDirectDraw7 *, ::HWND hwnd, ::DWORD dwFlags),
    ::IDirectDraw7 *that,
    ::HWND hwnd,
    ::DWORD dwFlags)
{
    log("IDirectDraw7::SetCooperativeLevel({} {} {})",
        static_cast<void *>(that),
        hwnd,
        SetCooperativeLevelFlags{dwFlags});

    const auto res = orig(that, hwnd, dwFlags);
    log("IDirectDraw7::SetCooperativeLevel res: {}", res);

    return res;
}

}

namespace IUnknown
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI QueryInterface(
    ::HRESULT(WINAPI *orig)(::IUnknown *, REFIID ridd, ::LPVOID FAR *ppvObj),
    ::IUnknown *that,
    REFIID ridd,
    ::LPVOID FAR *ppvObj)
{
    log("IUnknown::QueryInterface({}, {}, {}) orig: {}",
        static_cast<void *>(that),
        ridd,
        static_cast<void *>(ppvObj),
        reinterpret_cast<void *>(orig));

    const auto res = orig(that, ridd, ppvObj);
    log("IUnknown::QueryInterface res: {}", res);

    if (SUCCEEDED(res) && ppvObj && *ppvObj)
    {
        log("installing follow on hooks for {}", reinterpret_cast<void *>(*ppvObj));

        com_patch<^^IUnknown>(*ppvObj);

        if (::IsEqualGUID(ridd, IID_IDirectDraw))
        {
            com_patch<^^IDirectDraw>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirectDraw4))
        {
            com_patch<^^IDirectDraw4>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirectDraw7))
        {
            com_patch<^^IDirectDraw7>(*ppvObj);
        }
        else
        {
            log("unsupported follow on guid");
        }
    }

    return res;
}

}

[[= IATProxy]] ::HRESULT WINAPI
DirectDrawCreate(decltype(&::DirectDrawCreate) orig, ::GUID *lpGUID, ::LPDIRECTDRAW *lplpDD, ::IUnknown *pUnkOuter)
{
    const auto guid_str = lpGUID ? std::format("{}", *lpGUID) : "<null>";
    log("DirectDrawCreate called ({} {} {})", guid_str, static_cast<void *>(lplpDD), static_cast<void *>(pUnkOuter));

    const auto res = orig(lpGUID, lplpDD, pUnkOuter);
    log("DirectDrawCreate res: {}", res);

    if (res == DD_OK)
    {
        auto *dd = *lplpDD;
        auto *vtable = reinterpret_cast<::PROC *>(*reinterpret_cast<void **>(dd));

        log("direct draw object created: {} [vtable start: {}]", static_cast<void *>(dd), static_cast<void *>(vtable));

        com_patch<^^IUnknown>(dd);
        com_patch<^^IDirectDraw>(dd);
    }

    return res;
}

[[= IATProxy]] ::HRESULT WINAPI DirectDrawCreateEx(
    decltype(&::DirectDrawCreateEx) orig,
    ::GUID *lpGuid,
    ::LPVOID *lplpDD,
    REFIID iid,
    ::IUnknown *pUnkOuter)
{
    const auto guid_str = lpGuid ? std::format("{}", *lpGuid) : "<null>";
    log("DirectDrawCreateEx called ({} {} {} {})",
        guid_str,
        static_cast<void *>(lplpDD),
        iid,
        static_cast<void *>(pUnkOuter));

    const auto res = orig(lpGuid, lplpDD, iid, pUnkOuter);
    log("DirectDrawCreateEx res: {}", res);

    if (res == DD_OK)
    {
        auto *dd = *lplpDD;
        auto *vtable = reinterpret_cast<::PROC *>(*reinterpret_cast<void **>(dd));

        log("direct draw (ex) object created: {} [vtable start: {}]",
            static_cast<void *>(dd),
            static_cast<void *>(vtable));

        com_patch<^^IUnknown>(dd);

        if (::IsEqualGUID(iid, IID_IDirectDraw))
        {
            com_patch<^^IDirectDraw>(dd);
        }
        else if (::IsEqualGUID(iid, IID_IDirectDraw4))
        {
            com_patch<^^IDirectDraw4>(dd);
        }
        else if (::IsEqualGUID(iid, IID_IDirectDraw7))
        {
            com_patch<^^IDirectDraw7>(dd);
        }
        else
        {
            log("unsupported follow on guid");
        }
    }

    return res;
}
}

#include "core/dll_main/ddraw/ddraw_main.h"
