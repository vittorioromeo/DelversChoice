#pragma once

#include <mutex>

namespace nl
{
    template <typename Mutex, typename... Args>
    auto make_unique_lock(Mutex&& m, Args&&... args)
    {
        return std::unique_lock<std::remove_reference_t<Mutex>>(
            FWD(m), FWD(args)...);
    }
}
