#pragma once

#include <string_view>

namespace gems
{
struct IATPatchAnnotation
{
};
inline constexpr auto IATPatch = IATPatchAnnotation{};

struct IATProxyAnnotation
{
};
inline constexpr auto IATProxy = IATProxyAnnotation{};

struct COMDirectDrawProxyAnnotation
{
};
inline constexpr auto COMDirectDrawProxy = COMDirectDrawProxyAnnotation{};

}
