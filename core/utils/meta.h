#pragma once

#include <meta>
#include <vector>

namespace gems
{

template <auto namespce, auto annotation>
consteval auto find_functions_with_annotations() -> std::vector<std::meta::info>
{
    auto functions = std::vector<std::meta::info>{};

    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr auto func : std::define_static_array(std::meta::members_of(namespce, ctx)))
    {
        if constexpr (std::meta::is_function(func))
        {
            constexpr auto annotations =
                std::define_static_array(std::meta::annotations_of_with_type(func, annotation));

            if constexpr (!std::ranges::empty(annotations))
            {
                functions.push_back(func);
            }
        }
    }

    return functions;
}

template <class...>
struct Dbg;
}
