#pragma once

#include <format>

#include <windows.h>

#include <ddraw.h>

template <>
struct std::formatter<::GUID>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return ctx.begin();
    }

    auto format(const ::GUID &obj, std::format_context &ctx) const
    {
        if (::IsEqualGUID(obj, IID_IUnknown))
        {
            return std::format_to(ctx.out(), "IID_IUnknown");
        }
        else if (::IsEqualGUID(obj, IID_IDirectDraw))
        {
            return std::format_to(ctx.out(), "IID_IDirectDraw");
        }
        else if (::IsEqualGUID(obj, IID_IDirectDraw2))
        {
            return std::format_to(ctx.out(), "IID_IDirectDraw2");
        }
        else if (::IsEqualGUID(obj, IID_IDirectDraw3))
        {
            return std::format_to(ctx.out(), "IID_IDirectDraw3");
        }
        else if (::IsEqualGUID(obj, IID_IDirectDraw4))
        {
            return std::format_to(ctx.out(), "IID_IDirectDraw4");
        }
        else if (::IsEqualGUID(obj, IID_IDirectDraw7))
        {
            return std::format_to(ctx.out(), "IID_IDirectDraw7");
        }

        return std::format_to(
            ctx.out(),
            "{{{:08x}-{:04x}-{:04x}-{:02x}{:02x}-"
            "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}",
            obj.Data1,
            obj.Data2,
            obj.Data3,
            obj.Data4[0],
            obj.Data4[1],
            obj.Data4[2],
            obj.Data4[3],
            obj.Data4[4],
            obj.Data4[5],
            obj.Data4[6],
            obj.Data4[7]);
    }
};
