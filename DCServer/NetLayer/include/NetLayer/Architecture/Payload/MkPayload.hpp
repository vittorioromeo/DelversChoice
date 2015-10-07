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
        inline auto make_payload(const PayloadAddress& mAddress, Ts&&... mXs)
        {
            // Create payload and set address.
            Payload p{mAddress};

            // Seralize all passed arguments in the payload.
            ssvu::forArgs(
                [&p](auto&& x)
                {
                    serialize(p.data, FWD(x));
                },
                FWD(mXs)...);

            return p;
        }
    }
}
