#pragma once

#include "../Common/Common.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

namespace MPL = ::ecs::MPL;

namespace experiment
{
    template <typename TType>
    struct PcktBind
    {
        using Type = TType;
    };

    template <typename T>
    using PcktBindType = typename T::Type;

    template <typename... Ts>
    using PcktBinds = MPL::TypeList<Ts...>;

    template <typename TList>
    using PcktBindsTypes = MPL::Map<PcktBindType, TList>;
}
