#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include <windows.h>

namespace gems
{

inline auto get_temp(std::string_view filename) -> std::filesystem::path
{
    char tempPath[MAX_PATH]{};
    ::GetEnvironmentVariableA("TEMP", tempPath, MAX_PATH);
    return std::filesystem::path{tempPath} / filename;
}

template <class... T>
auto log(std::format_string<T...> fmt, T &&...args) -> void
{
    static auto mtx = std::mutex{};

    std::scoped_lock lock{mtx};

    const auto log_path = get_temp("log.txt");

    if (auto file = std::ofstream{log_path, std::ios::app}; file)
    {
        auto str = std::string{};
        std::format_to(std::back_inserter(str), fmt, std::forward<T>(args)...);
        file << str << std::endl;
    }
}

}
