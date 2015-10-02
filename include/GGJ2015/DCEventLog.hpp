#ifndef GGJ2015_DCEVENTLOG
#define GGJ2015_DCEVENTLOG

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
inline auto& getEventLogStream() noexcept
{
    static std::stringstream result;
    return result;
}

namespace Impl
{
    struct EventLog
    {
        template <typename T>
        inline auto operator<<(const T& mX)
        {
            getEventLogStream() << mX;
            ssvu::lo() << mX;
            return EventLog{};
        }
    };
}

inline auto eventLo() noexcept { return Impl::EventLog{}; }
}

#endif
