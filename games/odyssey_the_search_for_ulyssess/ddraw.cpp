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

namespace direct_draw
{
DDRAW_EXPORT[[= COMDirectDrawProxy]] ::HRESULT WINAPI QueryInterface(
    ::HRESULT(WINAPI *orig)(::IDirectDraw *, REFIID ridd, ::LPVOID FAR *ppvObj),
    ::IDirectDraw *that,
    REFIID ridd,
    ::LPVOID FAR *ppvObj)
{
    log("IDirectDraw::QueryInterface({}, {}, {}) orig: {}",
        static_cast<void *>(that),
        ridd,
        static_cast<void *>(ppvObj),
        reinterpret_cast<void *>(orig));

    const auto res = orig(that, ridd, ppvObj);
    log("IDirectDraw::QueryInterface res: {}", res);

    if (SUCCEEDED(res) && ppvObj && *ppvObj)
    {
        log("installing follow on hooks for {}", reinterpret_cast<void *>(*ppvObj));
        com_patch<^^direct_draw>(*ppvObj);
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

        com_patch<^^direct_draw>(dd);
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

        com_patch<^^direct_draw>(dd);
    }

    return res;
}
}

#include "core/dll_main/ddraw/ddraw_main.h"
