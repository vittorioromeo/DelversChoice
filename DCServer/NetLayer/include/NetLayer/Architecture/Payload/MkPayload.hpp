#pragma once

#include "../../Common/Common.hpp"
#include "./PayloadAddress.hpp"
#include "./Payload.hpp"
#include "../../Serialization/Serialization.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename... Ts>
        inline auto make_payload(const PayloadAddress& address, Ts&&... xs)
        {
            return Payload{address, nl::make_serialized(FWD(xs)...)};
        }
    }
}
