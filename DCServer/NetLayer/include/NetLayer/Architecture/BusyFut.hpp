#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename TF>
        auto make_async_and_launch(TF&& f)
        {
            return std::async(std::launch::async, FWD(f));
        }

        /// @brief Wrapper around an `std::future<void>` that runs a busy loop
        /// until explicitly stopped by the user.
        class busy_loop
        {
        private:
            std::atomic<bool> _alive{true}; // construct flag first!
            std::future<void> _future;

        public:
            template <typename TF>
            busy_loop(TF&& f)
                : _future{make_async_and_launch([this, f]
                      {
                          while(_alive)
                          {
                              f();
                          }
                      })}
            {
            }

            busy_loop(const busy_loop&) = delete;
            busy_loop& operator=(const busy_loop&) = delete;

            busy_loop(busy_loop&&) = default;
            busy_loop& operator=(busy_loop&&) = default;

            ~busy_loop()
            {
                if(_future.valid())
                {
                    _alive = false;
                    _future.get();
                }
            }

            void stop() noexcept { _alive = false; }
            bool busy() const noexcept { return _alive; }
        };
    }
}
