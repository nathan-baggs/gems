#pragma once

#include <format>
#include <sstream>

#include <unordered_map>
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

template <>
struct std::formatter<::HWND>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return ctx.begin();
    }

    auto format(const ::HWND &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "{}", reinterpret_cast<void *>(obj));
    }
};

struct SetCooperativeLevelFlags
{
    ::DWORD flags;
};

template <>
struct std::formatter<SetCooperativeLevelFlags>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return ctx.begin();
    }

    auto format(const SetCooperativeLevelFlags &obj, std::format_context &ctx) const
    {
        static auto lookup = std::unordered_map<::DWORD, std::string_view>{
            {DDSCL_ALLOWMODEX, "DDSCL_ALLOWMODEX"},
            {DDSCL_ALLOWREBOOT, "DDSCL_ALLOWREBOOT"},
            {DDSCL_CREATEDEVICEWINDOW, "DDSCL_CREATEDEVICEWINDOW"},
            {DDSCL_EXCLUSIVE, "DDSCL_EXCLUSIVE"},
            {DDSCL_FPUPRESERVE, "DDSCL_FPUPRESERVE"},
            {DDSCL_FPUSETUP, "DDSCL_FPUSETUP"},
            {DDSCL_FULLSCREEN, "DDSCL_FULLSCREEN"},
            {DDSCL_MULTITHREADED, "DDSCL_MULTITHREADED"},
            {DDSCL_NORMAL, "DDSCL_NORMAL"},
            {DDSCL_NOWINDOWCHANGES, "DDSCL_NOWINDOWCHANGES"},
            {DDSCL_SETDEVICEWINDOW, "DDSCL_SETDEVICEWINDOW"},
            {DDSCL_SETFOCUSWINDOW, "DDSCL_SETFOCUSWINDOW"},
        };

        auto strm = std::stringstream{};
        auto value = obj.flags;
        auto first = true;

        for (const auto &[flag, name] : lookup)
        {
            if (value == 0)
            {
                break;
            }

            if (value & flag)
            {
                if (!first)
                {
                    strm << " | ";
                }

                strm << name;
                first = false;

                value &= ~flag;
            }
        }

        if (value != 0)
        {
            strm << " | <unknown>";
        }

        return std::format_to(ctx.out(), "{:x} [{}]", obj.flags, strm.str());
    }
};
