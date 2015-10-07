#pragma once

#include "../../Common/Common.hpp"
#include "../../Architecture/ThreadSafeQueue.hpp"
#include "./PayloadAddress.hpp"
#include "./Payload.hpp"

namespace nl
{
    namespace Impl
    {
        using PayloadTSQueue = ThreadSafeQueue<Payload>;
    }
}
