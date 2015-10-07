#pragma once

#include "../Common/Common.hpp"

namespace experiment
{
    template <typename TIDType = nl::UInt32>
    struct Settings
    {
        using IDType = TIDType;
    };
}
