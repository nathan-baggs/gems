#include <atomic>
#include <libloaderapi.h>
#include <memory>
#include <meta>
#include <minwindef.h>
#include <source_location>
#include <unordered_map>

#include <windows.h>

#include <ddraw.h>
#include <winnt.h>

#include "core/utils/annotations.h"
#include "core/utils/error.h"
#include "core/utils/iat_patcher.h"
#include "core/utils/log.h"

using namespace std::literals;

extern "C"
{

[[= gems::IATPatch]] void WINAPI AcquireDDThreadLock();
[[= gems::IATPatch]] void WINAPI ReleaseDDThreadLock();
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateA(::LPDDENUMCALLBACKA, ::LPVOID);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateW(::LPDDENUMCALLBACKW, ::LPVOID);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateExA(::LPDDENUMCALLBACKEXA, ::LPVOID, ::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateExW(::LPDDENUMCALLBACKEXW, ::LPVOID, ::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreateClipper(::DWORD, ::LPDIRECTDRAWCLIPPER *, ::IUnknown *);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreate(::GUID *, ::LPDIRECTDRAW *, ::IUnknown *);
[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreateEx(::GUID *, ::LPVOID *, REFIID, ::IUnknown *);
[[= gems::IATPatch]] ::HRESULT WINAPI D3DParseUnknownCommand(::LPVOID, ::LPVOID *);
[[= gems::IATPatch]] ::HRESULT WINAPI GetSurfaceFromDC(::HDC, ::LPDIRECTDRAWSURFACE4 *, ::HDC *);
[[= gems::IATPatch]] ::HRESULT WINAPI DllCanUnloadNow();
[[= gems::IATPatch]] ::HRESULT WINAPI DllGetClassObject(REFCLSID, REFIID, void **);
[[= gems::IATPatch]] ::HRESULT WINAPI CompleteCreateSysmemSurface();
[[= gems::IATPatch]] ::HRESULT WINAPI DDGetAttachedSurfaceLcl(::DWORD, ::DWORD, ::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI DDInternalLock(::DWORD, ::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI DDInternalUnlock(::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI DSoundHelp(::DWORD, ::DWORD, ::DWORD);
[[= gems::IATPatch]] ::HRESULT WINAPI GetDDSurfaceLocal();
[[= gems::IATPatch]] ::HRESULT WINAPI GetOLEThunkData();
[[= gems::IATPatch]] ::HRESULT WINAPI RegisterSpecialCase();
[[= gems::IATPatch]] ::HRESULT WINAPI SetAppCompatData();
}

namespace gems::impl
{

auto g_kernel32 = ::HMODULE{};
auto g_proxied_ddraw = ::HMODULE{};
auto g_iat_entries = std::unordered_map<std::string_view, void *>{};
void *g_get_proc_address = {};

auto narrow(std::wstring_view str) -> std::string
{
    auto length = ::WideCharToMultiByte(
        CP_UTF8, WC_NO_BEST_FIT_CHARS, std::ranges::data(str), std::ranges::size(str), nullptr, 0, nullptr, nullptr);

    if (length == 0)
    {
        log("failed to get string length");
        return {};
    }

    auto res = std::string(length, '\0');

    length = ::WideCharToMultiByte(
        CP_UTF8,
        WC_NO_BEST_FIT_CHARS,
        std::ranges::data(str),
        std::ranges::size(str),
        std::ranges::data(res),
        std::ranges::size(res),
        nullptr,
        nullptr);

    if (length == 0)
    {
        log("failed to narrow string");
        return {};
    }

    return res;
}

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA
{
    ULONG Flags;                  // Reserved.
    PCUNICODE_STRING FullDllName; // The full path name of the DLL module.
    PCUNICODE_STRING BaseDllName; // The base file name of the DLL module.
    PVOID DllBase;                // A pointer to the base address for the DLL in memory.
    ULONG SizeOfImage;            // The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA
{
    ULONG Flags;                  // Reserved.
    PCUNICODE_STRING FullDllName; // The full path name of the DLL module.
    PCUNICODE_STRING BaseDllName; // The base file name of the DLL module.
    PVOID DllBase;                // A pointer to the base address for the DLL in memory.
    ULONG SizeOfImage;            // The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA
{
    LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
    LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

auto narrow(const UNICODE_STRING *str) -> std::string
{
    return narrow(std::wstring_view{str->Buffer, str->Length / sizeof(WCHAR)});
}

VOID CALLBACK LdrDllNotification(::ULONG NotificationReason, const LDR_DLL_NOTIFICATION_DATA *NotificationData, ::PVOID)
{
    switch (NotificationReason)
    {
        case 1:
        {
            const auto full_dll_name = narrow(NotificationData->Loaded.FullDllName);

            log("dll loaded: {}", full_dll_name);

            const auto module = ::LoadLibrary(full_dll_name.c_str());
            ensure(module != NULL, "failed to load module");

            const auto res = gems::impl::patch_out_module(module, "ddraw.dll", g_iat_entries);
            if (!res)
            {
                log("failed to patch {}: {}", full_dll_name, res.error());
            }

            break;
        }
        case 2:
        {
            log("dll unloaded: {}", narrow(NotificationData->Unloaded.FullDllName));
            break;
        }
    }
}

::FARPROC WINAPI GetProcAddress(::HMODULE hModule, ::LPCSTR lpProcName)
{
    ensure(g_proxied_ddraw != ::HMODULE{}, "proxied ddraw not set");

    if (hModule == g_proxied_ddraw)
    {
        log("routing {} to proxy", lpProcName);

        const auto lookup = g_iat_entries.find(lpProcName);
        ensure(lookup != std::ranges::cend(g_iat_entries), "{} missing from lookup", lpProcName);

        return reinterpret_cast<::FARPROC>(lookup->second);
    }

    return reinterpret_cast<decltype(&GetProcAddress)>(g_get_proc_address)(hModule, lpProcName);
}

auto try_load_iat_hooks(std::source_location loc = std::source_location::current()) -> void *
{
    const auto ntdll = ::LoadLibrary("ntdll.dll");
    ensure(ntdll != NULL, "failed to load ntdll");

    using PLDR_DLL_NOTIFICATION_FUNCTION = VOID(CALLBACK *)(ULONG, const LDR_DLL_NOTIFICATION_DATA *, PVOID);
    using LdrRegisterDllNotificationFn = int(CALLBACK *)(::ULONG, PLDR_DLL_NOTIFICATION_FUNCTION, ::PVOID, ::PVOID *);

    const auto LdrRegisterDllNotification =
        reinterpret_cast<LdrRegisterDllNotificationFn>(::GetProcAddress(ntdll, "LdrRegisterDllNotification"));
    ensure(LdrRegisterDllNotification != nullptr, "failed to find LdrRegisterDllNotification");

    log("registering loader callback");
    void *cookie = {};
    ensure(
        LdrRegisterDllNotification(0u, &gems::impl::LdrDllNotification, nullptr, &cookie) == 0,
        "could not register callback");

    g_kernel32 = load_system_dll("kernel32.dll");
    g_get_proc_address = reinterpret_cast<void *>(::GetProcAddress(g_kernel32, "GetProcAddress"));

    log("loading iat hooks, called from: {}", loc.function_name());

    void *ret_func = nullptr;

    const auto system_ddraw = load_system_dll("ddraw.dll");
    log("loaded system ddraw.dll: {}", static_cast<void *>(system_ddraw));

    static auto completed = std::atomic<bool>{false};
    ensure(!completed.exchange(true), "iat already loaded: {}", loc.function_name());

    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr auto func : std::define_static_array(std::meta::members_of(^^::, ctx)))
    {
        if constexpr (std::meta::is_function(func))
        {
            constexpr auto annotations =
                std::define_static_array(std::meta::annotations_of_with_type(func, ^^IATPatchAnnotation));

            if constexpr (!std::ranges::empty(annotations))
            {
                const auto func_name = std::meta::identifier_of(func);
                const auto func_name_str = std::string(std::ranges::data(func_name), std::ranges::size(func_name));
                const auto real_addr = reinterpret_cast<void *>(::GetProcAddress(system_ddraw, func_name_str.c_str()));
                ensure(real_addr, "{} not found", func_name_str);

                log("{}", func_name);
                g_iat_entries[func_name] = real_addr;

                if (std::string_view{loc.function_name()}.contains(func_name))
                {
                    ret_func = real_addr;
                }
            }
        }
    }

    log("found {} iat entries to patch", std::ranges::size(g_iat_entries));

    iat_patcher("ddraw.dll", g_iat_entries);

    log("iat patcher done, returning: {}", ret_func);

    iat_patcher("kernel32.dll", {{"GetProcAddress", reinterpret_cast<void *>(&gems::impl::GetProcAddress)}});

    return ret_func;
}
}

extern "C"
{

[[= gems::IATPatch]] void WINAPI AcquireDDThreadLock()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    reinterpret_cast<decltype(&AcquireDDThreadLock)>(orig_func)();
}

[[= gems::IATPatch]] void WINAPI ReleaseDDThreadLock()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    reinterpret_cast<decltype(&ReleaseDDThreadLock)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateA(::LPDDENUMCALLBACKA a, ::LPVOID b)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawEnumerateA)>(orig_func)(a, b);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateW(::LPDDENUMCALLBACKW a, ::LPVOID b)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawEnumerateW)>(orig_func)(a, b);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateExA(::LPDDENUMCALLBACKEXA a, ::LPVOID b, ::DWORD c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawEnumerateExA)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawEnumerateExW(::LPDDENUMCALLBACKEXW a, ::LPVOID b, ::DWORD c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawEnumerateExW)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreateClipper(::DWORD a, ::LPDIRECTDRAWCLIPPER *b, ::IUnknown *c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawCreateClipper)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreate(::GUID *a, ::LPDIRECTDRAW *b, ::IUnknown *c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawCreate)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DirectDrawCreateEx(::GUID *a, ::LPVOID *b, REFIID c, ::IUnknown *d)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DirectDrawCreateEx)>(orig_func)(a, b, c, d);
}

[[= gems::IATPatch]] ::HRESULT WINAPI D3DParseUnknownCommand(::LPVOID a, ::LPVOID *b)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&D3DParseUnknownCommand)>(orig_func)(a, b);
}

[[= gems::IATPatch]] ::HRESULT WINAPI GetSurfaceFromDC(::HDC a, ::LPDIRECTDRAWSURFACE4 *b, ::HDC *c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&GetSurfaceFromDC)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DllCanUnloadNow()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DllCanUnloadNow)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI DllGetClassObject(REFCLSID a, REFIID b, void **c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DllGetClassObject)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI CompleteCreateSysmemSurface()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&CompleteCreateSysmemSurface)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI DDGetAttachedSurfaceLcl(::DWORD a, ::DWORD b, ::DWORD c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DDGetAttachedSurfaceLcl)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DDInternalLock(::DWORD a, ::DWORD b)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DDInternalLock)>(orig_func)(a, b);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DDInternalUnlock(::DWORD a)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DDInternalUnlock)>(orig_func)(a);
}

[[= gems::IATPatch]] ::HRESULT WINAPI DSoundHelp(::DWORD a, ::DWORD b, ::DWORD c)
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&DSoundHelp)>(orig_func)(a, b, c);
}

[[= gems::IATPatch]] ::HRESULT WINAPI GetDDSurfaceLocal()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&GetDDSurfaceLocal)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI GetOLEThunkData()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&GetOLEThunkData)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI RegisterSpecialCase()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&RegisterSpecialCase)>(orig_func)();
}

[[= gems::IATPatch]] ::HRESULT WINAPI SetAppCompatData()
{
    const auto orig_func = gems::impl::try_load_iat_hooks();
    return reinterpret_cast<decltype(&SetAppCompatData)>(orig_func)();
}

::BOOL WINAPI DllMain(::HMODULE module, ::DWORD reason, void *)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        ::DisableThreadLibraryCalls(module);
        gems::log("DllMain called for ddraw.dll");

        gems::impl::g_proxied_ddraw = module;
    }
    return TRUE;
}
}
