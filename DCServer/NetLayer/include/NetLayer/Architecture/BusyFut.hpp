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

        public:
            template <typename TF>
            BusyFut(TF&& mFn)
                : fut{std::async(std::launch::async, [this, mFn]
                      {
                          while(wrapperAlive)
                          {
                              // NL_DEBUGLO() << "waiting\n";
                              mFn();
                          }

                          //      NL_DEBUGLO() << (wrapperAlive ? "alive" :
                          //      "dead!!");
                      })}
            {
            }
            /*
                    BusyFut(const BusyFut& mX) = delete;
                    BusyFut(BusyFut&& mX) : wrapperAlive(true),
               fut(std::move(mX.fut))
                    {
                       // std::cout << "move ctor\n";
                        mX.wrapperAlive = false;
                    }
            */
            /*    BusyFut& operator=(const BusyFut&) = delete;
                BusyFut& operator=(BusyFut&& mX)
                {
                    mX.wrapperAlive = false;
                    fut = std::move(mX.fut);
                    return *this;
                }*/

            ~BusyFut()
            {
                // std::cout << "dtor\n";

                if(fut.valid())
                {
                    // std::cout << "dtor kill\n";
                    wrapperAlive = false;
                    fut.get();
                }

                // block, so it sees wrapperAlive before it is
                // NL_DEBUGLO() << "beforeget";
                // destroyed.
                // NL_DEBUGLO() << "aftgerget";
            }

            void stop() { wrapperAlive = false; }
            bool isBusy() const { return wrapperAlive; }
        };
    }
}
