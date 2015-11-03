#pragma once

#include <cstddef>
#include <chrono>

namespace nl
{
    template <typename TFDuration, typename TF>
    auto retry(std::size_t times, TF&& f, TFDuration&& f_duration)
    {
        for(auto i(0u); i < times; ++i)
        {
            if(f())
            {
                return true;
            }

            std::this_thread::sleep_for(f_duration(i, times));
        }

        return false;
    }

    template <typename TF>
    auto retry(std::size_t times, TF&& f)
    {
        return retry(times, f, [](auto itr, auto)
            {
                return 100ms * itr;
            });
    }
}
