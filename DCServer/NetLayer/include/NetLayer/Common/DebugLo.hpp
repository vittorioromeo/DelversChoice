#pragma once

namespace nl
{
    inline auto& debugLo() noexcept
    {
#if 1
        return ::ssvu::lo();
#else
        return ::ssvu::loNull();
#endif
    }
}
