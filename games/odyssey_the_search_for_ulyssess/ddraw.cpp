#include "ddraw_export.h"

#include <windows.h>

#include <ddraw.h>

#include "core/utils/annotations.h"
#include "core/utils/error.h"
#include "core/utils/formatter.h"
#include "core/utils/log.h"

namespace gems
{

[[= IATProxy]] ::HRESULT WINAPI
DirectDrawCreate(decltype(&::DirectDrawCreate) orig, ::GUID *lpGUID, ::LPDIRECTDRAW *lplpDD, ::IUnknown *pUnkOuter)
{
    const auto guid_str = lpGUID ? std::format("{}", *lpGUID) : "<null>";
    log("DirectDrawCreate called ({} {} {})", guid_str, static_cast<void *>(lplpDD), static_cast<void *>(pUnkOuter));

    const auto res = orig(lpGUID, lplpDD, pUnkOuter);
    log("DirectDrawCreate res: {}", res);

    return res;
}

}

#include "core/dll_main/ddraw/ddraw_main.h"
