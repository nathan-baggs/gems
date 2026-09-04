#pragma once

#include <windows.h>

#include "core/utils/error.h"

namespace gems
{

struct AutoWriter
{
    AutoWriter(void *address, ::SIZE_T size)
        : address{address}
        , size{size}
        , protection{}
    {
        ensure(
            ::VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &protection) != 0,
            "failed to change memory protection");
    }

    ~AutoWriter()
    {
        ::VirtualProtect(address, size, protection, &protection);
    }

    void *address;
    ::SIZE_T size;
    ::DWORD protection;
};

}
