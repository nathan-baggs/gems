#pragma once

#include <cstdlib>
#include <format>
#include <iostream>
#include <ostream>
#include <print>
#include <utility>

#include "core/utils/log.h"

namespace gems
{

template <class... T>
auto die(std::format_string<T...> fmt, T &&...args) -> void
{
    log(fmt, std::forward<T>(args)...);
    std::println(fmt, std::forward<T>(args)...);
    std::cout << std::flush;

    std::exit(1);
}

template <class... T>
auto ensure(bool cond, std::format_string<T...> fmt, T &&...args) -> void
{
    if (!cond)
    {
        die(fmt, std::forward<T>(args)...);
    }
}
}
