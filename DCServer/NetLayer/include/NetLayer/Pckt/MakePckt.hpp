#pragma once

#include "../Common/Common.hpp"
#include "../Pckt/PcktImpl.hpp"

namespace nl
{
    template <typename T, typename... Ts>
    auto make_pckt(Ts&&... fields)
    {
        T res(nl::init_fields{}, FWD(fields)...);
        return res;
    }
}
