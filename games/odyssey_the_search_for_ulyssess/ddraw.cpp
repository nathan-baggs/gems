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

namespace IUnknown
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI QueryInterface(
    ::HRESULT(WINAPI *orig)(::IUnknown *, REFIID ridd, ::LPVOID FAR *ppvObj),
    ::IUnknown *that,
    REFIID ridd,
    ::LPVOID FAR *ppvObj);

}

namespace IDirect3D3
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateDevice(
    ::HRESULT(WINAPI *orig)(::IDirect3D3 *, REFCLSID, ::LPDIRECTDRAWSURFACE4, ::LPDIRECT3DDEVICE3 *, ::LPUNKNOWN),
    ::IDirect3D3 *that,
    REFCLSID rclsid,
    ::LPDIRECTDRAWSURFACE4 lpDDS,
    ::LPDIRECT3DDEVICE3 *lplpD3DDevice,
    ::LPUNKNOWN pUnkOuter);
}

namespace IDirect3D3Device3
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI
BeginScene(::HRESULT(WINAPI *orig)(::IDirect3DDevice3 *), ::IDirect3DDevice3 *that);

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI
EndScene(::HRESULT(WINAPI *orig)(::IDirect3DDevice3 *), ::IDirect3DDevice3 *that);
}

namespace IDirectDrawSurface
{
auto rect_string(const ::RECT *rect) -> std::string
{
    return rect ? std::format("[{},{}-{},{}]", rect->left, rect->top, rect->right, rect->bottom) : "<null>";
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI Blt(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::LPRECT, ::LPDIRECTDRAWSURFACE, ::LPRECT, ::DWORD, ::LPDDBLTFX),
    ::IDirectDrawSurface *that,
    ::LPRECT dest_rect,
    ::LPDIRECTDRAWSURFACE source,
    ::LPRECT source_rect,
    ::DWORD flags,
    ::LPDDBLTFX fx)
{
    log("IDirectDrawSurface::Blt(dest={} rect={} source={} rect={} flags=0x{:08x} fx={})",
        static_cast<void *>(that),
        rect_string(dest_rect),
        static_cast<void *>(source),
        rect_string(source_rect),
        flags,
        static_cast<void *>(fx));
    const auto res = orig(that, dest_rect, source, source_rect, flags, fx);
    log("IDirectDrawSurface::Blt res: {}", res);
    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI BltFast(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::DWORD, ::DWORD, ::LPDIRECTDRAWSURFACE, ::LPRECT, ::DWORD),
    ::IDirectDrawSurface *that,
    ::DWORD x,
    ::DWORD y,
    ::LPDIRECTDRAWSURFACE source,
    ::LPRECT source_rect,
    ::DWORD flags)
{
    log("IDirectDrawSurface::BltFast(dest={} x={} y={} source={} rect={} flags=0x{:08x})",
        static_cast<void *>(that), x, y, static_cast<void *>(source), rect_string(source_rect), flags);
    const auto res = orig(that, x, y, source, source_rect, flags);
    log("IDirectDrawSurface::BltFast res: {}", res);
    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI Flip(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::LPDIRECTDRAWSURFACE, ::DWORD),
    ::IDirectDrawSurface *that,
    ::LPDIRECTDRAWSURFACE target_override,
    ::DWORD flags)
{
    log("IDirectDrawSurface::Flip(surface={} target={} flags=0x{:08x})",
        static_cast<void *>(that), static_cast<void *>(target_override), flags);
    const auto res = orig(that, target_override, flags);
    log("IDirectDrawSurface::Flip res: {}", res);
    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI GetSurfaceDesc(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::LPDDSURFACEDESC),
    ::IDirectDrawSurface *that,
    ::LPDDSURFACEDESC desc)
{
    const auto res = orig(that, desc);
    log("IDirectDrawSurface::GetSurfaceDesc(surface={}) res: {} {}",
        static_cast<void *>(that), res, desc ? std::format("{}", *desc) : "<null>");
    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI Lock(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::LPRECT, ::LPDDSURFACEDESC, ::DWORD, ::HANDLE),
    ::IDirectDrawSurface *that,
    ::LPRECT rect,
    ::LPDDSURFACEDESC desc,
    ::DWORD flags,
    ::HANDLE event)
{
    const auto res = orig(that, rect, desc, flags, event);
    log("IDirectDrawSurface::Lock(surface={} rect={} flags=0x{:08x} event={}) res: {} {}",
        static_cast<void *>(that), rect_string(rect), flags, event, res, desc ? std::format("{}", *desc) : "<null>");
    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI Unlock(
    ::HRESULT(WINAPI *orig)(::IDirectDrawSurface *, ::LPVOID),
    ::IDirectDrawSurface *that,
    ::LPVOID data)
{
    log("IDirectDrawSurface::Unlock(surface={} data={})", static_cast<void *>(that), data);
    const auto res = orig(that, data);
    log("IDirectDrawSurface::Unlock res: {}", res);
    return res;
}
}

namespace IDirectDraw
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    ::HRESULT(WINAPI *orig)(::IDirectDraw *, ::HWND hwnd, ::DWORD dwFlags),
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

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateSurface(
    ::HRESULT(WINAPI *orig)(::IDirectDraw *, ::LPDDSURFACEDESC, ::LPDIRECTDRAWSURFACE *, ::IUnknown *),
    ::IDirectDraw *that,
    ::LPDDSURFACEDESC desc,
    ::LPDIRECTDRAWSURFACE *surface,
    ::IUnknown *outer)
{
    const auto desc_str = desc ? std::format("{}", *desc) : "<null>";

    log("IDirectDraw::CreateSurface({} {} {} {}",
        static_cast<void *>(that),
        desc_str,
        static_cast<void *>(surface),
        static_cast<void *>(outer));

    const auto res = orig(that, desc, surface, outer);

    if (SUCCEEDED(res) && surface && *surface)
    {
        com_patch<^^IUnknown>(*surface);
        com_patch<^^IDirectDrawSurface>(*surface);
    }
    log("IDirectDraw::CreateSurface res: {}", res);

    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI GetDisplayMode(
    ::HRESULT(WINAPI *orig)(::IDirectDraw *, ::LPDDSURFACEDESC),
    ::IDirectDraw *that,
    ::LPDDSURFACEDESC desc)
{
    const auto res = orig(that, desc);
    const auto desc_str = desc ? std::format("{}", *desc) : "<null>";

    log("IDirectDraw::GetDisplayMode({}) res: {} {}", static_cast<void *>(that), res, desc_str);

    return res;
}

}

namespace IDirectDraw2
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateSurface(
    ::HRESULT(WINAPI *orig)(::IDirectDraw2 *, ::LPDDSURFACEDESC, ::LPDIRECTDRAWSURFACE *, ::IUnknown *),
    ::IDirectDraw2 *that,
    ::LPDDSURFACEDESC desc,
    ::LPDIRECTDRAWSURFACE *surface,
    ::IUnknown *outer)
{
    const auto desc_str = desc ? std::format("{}", *desc) : "<null>";

    log("IDirectDraw2::CreateSurface({} {} {} {}",
        static_cast<void *>(that),
        desc_str,
        static_cast<void *>(surface),
        static_cast<void *>(outer));

    const auto res = orig(that, desc, surface, outer);

    if (SUCCEEDED(res) && surface && *surface)
    {
        com_patch<^^IUnknown>(*surface);
        com_patch<^^IDirectDrawSurface>(*surface);
    }
    log("IDirectDraw2::CreateSurface res: {}", res);

    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    ::HRESULT(WINAPI *orig)(::IDirectDraw2 *, ::HWND hwnd, ::DWORD dwFlags),
    ::IDirectDraw2 *that,
    ::HWND hwnd,
    ::DWORD dwFlags)
{
    log("IDirectDraw2::SetCooperativeLevel({} {} {})",
        static_cast<void *>(that),
        hwnd,
        SetCooperativeLevelFlags{dwFlags});

    const auto res = orig(that, hwnd, dwFlags);
    log("IDirectDraw2::SetCooperativeLevel res: {}", res);

    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI GetDisplayMode(
    ::HRESULT(WINAPI *orig)(::IDirectDraw2 *, ::LPDDSURFACEDESC),
    ::IDirectDraw2 *that,
    ::LPDDSURFACEDESC desc)
{
    const auto res = orig(that, desc);
    const auto desc_str = desc ? std::format("{}", *desc) : "<null>";

    log("IDirectDraw2::GetDisplayMode({}) res: {} {}", static_cast<void *>(that), res, desc_str);

    return res;
}

}

namespace IDirectDraw4
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    ::HRESULT(WINAPI *orig)(::IDirectDraw4 *, ::HWND hwnd, ::DWORD dwFlags),
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

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateSurface(
    ::HRESULT(WINAPI *orig)(::IDirectDraw4 *, ::LPDDSURFACEDESC2, ::LPDIRECTDRAWSURFACE4 *, ::IUnknown *),
    ::IDirectDraw4 *that,
    ::LPDDSURFACEDESC2 desc,
    ::LPDIRECTDRAWSURFACE4 *surface,
    ::IUnknown *outer)
{
    const auto desc_str =
        desc ? std::format(
                   "DDSURFACEDESC2{{size={} flags=0x{:08x} width={} height={} caps=0x{:08x} caps2=0x{:08x}}}",
                   desc->dwSize,
                   desc->dwFlags,
                   desc->dwWidth,
                   desc->dwHeight,
                   desc->ddsCaps.dwCaps,
                   desc->ddsCaps.dwCaps2)
             : "<null>";

    log("IDirectDraw4::CreateSurface({} {} {} {}",
        static_cast<void *>(that),
        desc_str,
        static_cast<void *>(surface),
        static_cast<void *>(outer));

    const auto res = orig(that, desc, surface, outer);

    if (SUCCEEDED(res) && surface && *surface)
    {
        com_patch<^^IUnknown>(*surface);
    }
    log("IDirectDraw4::CreateSurface res: {}", res);

    return res;
}

}

namespace IDirectDraw7
{

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI SetCooperativeLevel(
    ::HRESULT(WINAPI *orig)(::IDirectDraw7 *, ::HWND hwnd, ::DWORD dwFlags),
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

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateSurface(
    ::HRESULT(WINAPI *orig)(::IDirectDraw7 *, ::LPDDSURFACEDESC2, ::LPDIRECTDRAWSURFACE7 *, ::IUnknown *),
    ::IDirectDraw7 *that,
    ::LPDDSURFACEDESC2 desc,
    ::LPDIRECTDRAWSURFACE7 *surface,
    ::IUnknown *outer)
{
    const auto desc_str =
        desc ? std::format(
                   "DDSURFACEDESC2{{size={} flags=0x{:08x} width={} height={} caps=0x{:08x} caps2=0x{:08x}}}",
                   desc->dwSize,
                   desc->dwFlags,
                   desc->dwWidth,
                   desc->dwHeight,
                   desc->ddsCaps.dwCaps,
                   desc->ddsCaps.dwCaps2)
             : "<null>";

    log("IDirectDraw7::CreateSurface({} {} {} {}",
        static_cast<void *>(that),
        desc_str,
        static_cast<void *>(surface),
        static_cast<void *>(outer));

    const auto res = orig(that, desc, surface, outer);

    if (SUCCEEDED(res) && surface && *surface)
    {
        com_patch<^^IUnknown>(*surface);
    }
    log("IDirectDraw7::CreateSurface res: {}", res);

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
        else if (::IsEqualGUID(ridd, IID_IDirectDraw2))
        {
            com_patch<^^IDirectDraw2>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirectDrawSurface))
        {
            com_patch<^^IDirectDrawSurface>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirectDraw4))
        {
            com_patch<^^IDirectDraw4>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirectDraw7))
        {
            com_patch<^^IDirectDraw7>(*ppvObj);
        }
        else if (::IsEqualGUID(ridd, IID_IDirect3D3))
        {
            com_patch<^^IDirect3D3>(*ppvObj);
        }
        else
        {
            log("unsupported follow on guid");
        }
    }

    return res;
}

}

namespace IDirect3D3Device3
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI
BeginScene(::HRESULT(WINAPI *orig)(::IDirect3DDevice3 *), ::IDirect3DDevice3 *that)
{
    log("IDirect3D3Device3::BeginScene({})", static_cast<void *>(that));

    const auto res = orig(that);
    log("IDirect3D3Device3::BeginScene res: {}", res);

    return res;
}

DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI
EndScene(::HRESULT(WINAPI *orig)(::IDirect3DDevice3 *), ::IDirect3DDevice3 *that)
{
    log("IDirect3D3Device3::EndScene({})", static_cast<void *>(that));

    const auto res = orig(that);
    log("IDirect3D3Device3::EndScene res: {}", res);

    return res;
}
}

namespace IDirect3D3
{
DDRAW_EXPORT[[= COMProxy]] ::HRESULT WINAPI CreateDevice(
    ::HRESULT(WINAPI *orig)(::IDirect3D3 *, REFCLSID, ::LPDIRECTDRAWSURFACE4, ::LPDIRECT3DDEVICE3 *, ::LPUNKNOWN),
    ::IDirect3D3 *that,
    REFCLSID rclsid,
    ::LPDIRECTDRAWSURFACE4 lpDDS,
    ::LPDIRECT3DDEVICE3 *lplpD3DDevice,
    ::LPUNKNOWN pUnkOuter)
{
    log("IDirect3D3::CreateDevice({} {} {} {} {})",
        static_cast<void *>(that),
        rclsid,
        static_cast<void *>(lpDDS),
        static_cast<void *>(lplpD3DDevice),
        static_cast<void *>(pUnkOuter));

    const auto res = orig(that, rclsid, lpDDS, lplpD3DDevice, pUnkOuter);
    log("IDirect3D3::CreateDevice res: {}", res);

    if (SUCCEEDED(res) && lplpD3DDevice && *lplpD3DDevice)
    {
        log("installing follow on hooks for {}", reinterpret_cast<void *>(*lplpD3DDevice));

        com_patch<^^IUnknown>(*lplpD3DDevice);
        // com_patch<^^IDirect3D3Device3>(*lplpD3DDevice);
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
        else if (::IsEqualGUID(iid, IID_IDirectDraw2))
        {
            com_patch<^^IDirectDraw2>(dd);
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
            log("unsupported follow on guid: {}", iid);
        }
    }

    return res;
}

[[= IATProxy]] ::HRESULT WINAPI
D3DParseUnknownCommand(::HRESULT(WINAPI *orig)(::LPVOID, ::LPVOID *), ::LPVOID lpCmd, ::LPVOID *lpRetCmd)
{
    log("D3DParseUnknownCommand({}, {})", lpCmd, static_cast<void *>(lpRetCmd));
    const auto res = orig(lpCmd, lpRetCmd);

    log("D3DParseUnknownCommand res = {}", res);

    return res;
}

}

#include "core/dll_main/ddraw/ddraw_main.h"
