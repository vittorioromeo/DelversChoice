#pragma once

#include "../Common/Common.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

namespace MPL = ::ecs::MPL;

namespace experiment
{
    namespace impl
    {
        template <typename T>
        struct pckt_bind_type
        {
            using type = T;
        };

        template <typename T>
        constexpr auto pckt_bind()
        {
            return impl::pckt_bind_type<T>{};
        }
    }

    template <typename... Ts>
    constexpr auto pckt_binds()
    {
        return MPL::TypeList<typename decltype(
            impl::pckt_bind<Ts>())::type...>{};
    }
}
