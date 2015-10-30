#pragma once

#include "../Common/Common.hpp"

namespace nl
{
    template <typename TFDuration, typename TF>
    bool retry(std::size_t mTimes, TF&& mFn, TFDuration&& mFDuration)
    {
        for(auto i(0u); i < mTimes; ++i)
        {
            if(mFn())
            {
                return true;
            }

            std::this_thread::sleep_for(mFDuration(i, mTimes));

            if(i > 4)
            {
                // ::nl::debugLo() << i << "th retry...\n";
            }
        }

        // ::nl::debugLo() << "failed!...\n";
        return false;
    }

    template <typename TF>
    bool retry(std::size_t mTimes, TF&& mFn)
    {
        return retry(mTimes, mFn, [](auto itr, auto)
            {
                return 100ms * itr;
            });
    }
}
