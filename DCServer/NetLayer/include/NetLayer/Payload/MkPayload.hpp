#pragma once

#include "../Common/Common.hpp"
#include "./PayloadAddress.hpp"
#include "./PayloadImpl.hpp"
#include "../Serialization/Serialization.hpp"

namespace nl
{
    template <typename... Ts>
    inline auto make_payload(const PAddress& address, Ts&&... xs)
    {
        return Payload{address, make_serialized(FWD(xs)...)};
    }
}
