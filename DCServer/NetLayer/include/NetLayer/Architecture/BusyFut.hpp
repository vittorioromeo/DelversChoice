#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"

namespace nl
{
namespace Impl
{
    class BusyFut
    {
    private:
        std::atomic<bool> wrapperAlive{true}; // construct flag first!
        std::future<void> fut;

        template <typename TF>
        void loop(const TF& mFn)
        {
            while(wrapperAlive) {
                // NL_DEBUGLO() << "waiting\n";
                mFn();
            }

            // NL_DEBUGLO() << (wrapperAlive ? "alive" : "dead!!");
        }

    public:
        template <typename TF>
        BusyFut(const TF& mFn)
            : fut{std::async(std::launch::async, [this, mFn]
              {
                  loop(mFn);
              })}
        {
        }

        BusyFut(BusyFut&& mX) : wrapperAlive(true), fut(std::move(mX.fut)) {}

        ~BusyFut()
        {
            wrapperAlive = false;
            // NL_DEBUGLO() << "beforeget";
            fut.get(); // block, so it sees wrapperAlive before it is
                       // destroyed.
            // NL_DEBUGLO() << "aftgerget";
        }

        void stop() { wrapperAlive = false; }
        bool isBusy() const { return wrapperAlive; }
    };
}
}
