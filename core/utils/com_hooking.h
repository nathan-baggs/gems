#pragma once

#include <cstdint>
#include <meta>
#include <ranges>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "core/utils/annotations.h"
#include "core/utils/error.h"
#include "core/utils/log.h"
#include "core/utils/meta.h"

namespace gems
{

namespace impl
{

constexpr std::string_view g_ddraw_com_layout[] = {
    "QueryInterface",
    "AddRef",
    "Release",
    "Compact",
    "CreateClipper",
    "CreatePalette",
    "CreateSurface",
    "DuplicateSurface",
    "EnumDisplayModes",
    "EnumSurfaces",
    "FlipToGDISurface",
    "GetCaps",
    "GetDisplayMode",
    "GetFourCCCodes",
    "GetGDISurface",
    "GetMonitorFrequency",
    "GetScanLine",
    "GetVerticalBlankStatus",
    "Initialize",
    "RestoreDisplayMode",
    "SetCooperativeLevel",
    "SetDisplayMode",
    "WaitForVerticalBlank",
    "GetAvailableVidMem",
    "GetSurfaceFromDC",
    "RestoreAllSurfaces",
    "TestCooperativeLevel",
    "GetDeviceIdentifier",
    "StartModeTest",
    "EvaluateMode",
};

struct Hook
{
    void *vtable;
    std::size_t index;
    ::PROC orig_func;
    ::PROC hook_func;
};

auto g_ddraw_hooks = std::vector<Hook>{};

template <std::size_t Index, class R, class Orig, class... Args>
__declspec(dllexport) auto WINAPI com_trampoline(void *that, Args... args) -> R
{
    const auto *com_obj = reinterpret_cast<::PROC **>(that);
    const auto *com_vtable = *com_obj;

    const auto hook_find = std::ranges::find_if(
        impl::g_ddraw_hooks, [com_vtable](const auto &e) { return e.vtable == com_vtable && e.index == Index; });
    ensure(
        hook_find != std::ranges::cend(impl::g_ddraw_hooks),
        "could not find hook index: {} vtable: {} that: {}",
        Index,
        reinterpret_cast<const void *>(com_vtable),
        that);

    const auto &hook = *hook_find;

    using COMCallType = R(WINAPI *)(Orig, void *, Args...);

    return reinterpret_cast<COMCallType>(hook.hook_func)(reinterpret_cast<Orig>(hook.orig_func), that, args...);
}

template <std::size_t Index, class R, class ArgsTuple>
consteval auto build_com_trampoline(std::meta::info orig_func_ptr) -> std::meta::info
{
    auto args_meta = std::meta::template_arguments_of(std::meta::dealias(^^ArgsTuple));
    args_meta.insert(std::ranges::begin(args_meta), orig_func_ptr);
    args_meta.insert(std::ranges::begin(args_meta), ^^R);
    args_meta.insert(std::ranges::begin(args_meta), std::meta::reflect_constant(Index));

    return std::meta::substitute(^^com_trampoline, args_meta);
}

template <class T>
struct FuncInfo;

template <class R, class OrigFunc, class That, class... Args>
struct FuncInfo<R WINAPI(OrigFunc, That, Args...)>
{
    using ret_type = R;
    using orig_func = OrigFunc;
    using args = std::tuple<Args...>;
};

}

template <auto namespce>
auto com_patch(::PROC *vtable)
{
    static constexpr auto proxy_functions =
        std::define_static_array(find_functions_with_annotations<namespce, ^^COMDirectDrawProxyAnnotation>());

    template for (constexpr auto func : proxy_functions)
    {
        constexpr auto func_name = std::meta::identifier_of(func);
        constexpr auto find = std::ranges::find(impl::g_ddraw_com_layout, func_name);

        if constexpr (find != std::ranges::cend(impl::g_ddraw_com_layout))
        {
            constexpr auto index = std::distance(std::ranges::begin(impl::g_ddraw_com_layout), find);

            using RetType = impl::FuncInfo<typename[:std::meta::type_of(func):]>::ret_type;
            using OrigFuncType = impl::FuncInfo<typename[:std::meta::type_of(func):]>::orig_func;
            using ArgsTupleType = impl::FuncInfo<typename[:std::meta::type_of(func):]>::args;

            constexpr auto trampoline =
                impl::build_com_trampoline<index, RetType, ArgsTupleType>(std::meta::dealias(^^OrigFuncType));

            const auto orig_func = std::exchange(vtable[index], reinterpret_cast<::PROC>(&[:trampoline:]));

            impl::g_ddraw_hooks.push_back(
                impl::Hook{
                    .vtable = vtable,
                    .index = index,
                    .orig_func = orig_func,
                    .hook_func = reinterpret_cast<::PROC>(&[:func:]),
                });

            auto &new_hook = impl::g_ddraw_hooks.back();
            log("new com hook: {} {} -> {}, index: {} vtable: {}",
                func_name,
                reinterpret_cast<void *>(new_hook.orig_func),
                reinterpret_cast<void *>(new_hook.hook_func),
                new_hook.index,
                new_hook.vtable);
        }
    }
}
}
