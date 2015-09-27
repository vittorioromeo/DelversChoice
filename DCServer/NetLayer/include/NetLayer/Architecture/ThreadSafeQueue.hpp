#pragma once

#include <mutex>
#include <condition_variable>
#include "../Common/Common.hpp"

namespace nl
{
    namespace Impl
    {
        template <typename T>
        class ThreadSafeQueue
        {
        private:
            std::deque<T> data;
            mutable std::timed_mutex mtx;
            std::condition_variable_any cv;
            using UniqueLock = std::unique_lock<decltype(mtx)>;

        public:
            // TODO: remove
            ~ThreadSafeQueue() { NL_DEBUGLO() << "~queue"; }

            template <typename... TArgs>
            void enqueue(TArgs&&... mArgs)
            {
                {
                    // TODO: make_mutex?
                    UniqueLock l(mtx);
                    data.emplace_front(FWD(mArgs)...);
                }

                // Notify one thread waiting to `dequeue`.
                cv.notify_one();
            }

            // TODO:
            template <typename... TArgs>
            bool try_enqueue(TArgs&&... mArgs)
            {
                UniqueLock l(mtx, std::defer_lock_t{});

                if(l.try_lock_for(100ms)) {
                    data.emplace_front(FWD(mArgs)...);

                    // TODO: ???
                    // Notify one thread waiting to `dequeue`.
                    cv.notify_one();

                    return true;
                }

                return false;
            }

            T dequeue()
            {
                UniqueLock l(mtx);

                // Release the lock and wait until `data` is not empty.
                cv.wait(l, [this]
                        {
                            return !data.empty();
                        });

                auto result(data.back());
                data.pop_back();
                return result;
            }

            // TODO:
            bool try_dequeue(T& mTarget)
            {
                UniqueLock l(mtx);

                // Release the lock and wait until `data` is not empty.
                if(cv.wait_for(l, 100ms, [this]
                               {
                                   return !data.empty();
                               }))
                {
                    auto result(data.back());
                    data.pop_back();
                    mTarget = result;
                    return true;
                }

                return false;
            }

            auto size() const
            {
                UniqueLock l(mtx);
                return data.size();
            }

            auto empty() const
            {
                UniqueLock l(mtx);
                return data.empty();
            }
        };
    }
}
