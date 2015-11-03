#pragma once

#include <mutex>
#include <condition_variable>
#include "../Common/Common.hpp"
#include "../Utils/MkUniqueLock.hpp"

namespace nl
{
    namespace impl
    {
        template <typename T>
        class ThreadSafeQueue
        {
        private:
            std::deque<T> data;
            mutable std::timed_mutex mtx;
            std::condition_variable_any cv;

            template <typename... TArgs>
            void enqueue_impl(TArgs&&... mArgs)
            {
                data.emplace_front(FWD(mArgs)...);

                // Notify one thread waiting to `dequeue`.
                cv.notify_one();
            }

            auto dequeue_impl()
            {
                auto result(data.back());
                data.pop_back();
                return result;
            }

        public:
            template <typename... TArgs>
            void enqueue(TArgs&&... mArgs)
            {
                auto l(make_unique_lock(mtx));
                enqueue_impl(FWD(mArgs)...);
            }

            template <typename TDuration, typename... TArgs>
            bool try_enqueue_for(const TDuration& mDuration, TArgs&&... mArgs)
            {
                auto l(make_unique_lock(mtx, std::defer_lock));

                if(l.try_lock_for(mDuration))
                {
                    enqueue_impl(FWD(mArgs)...);
                    return true;
                }

                return false;
            }

            T dequeue()
            {
                auto l(make_unique_lock(mtx));

                // Release the lock and wait until `data` is not empty.
                cv.wait(l, [this]
                    {
                        return !data.empty();
                    });

                return dequeue_impl();
            }

            template <typename TDuration>
            bool try_dequeue_for(const TDuration& mDuration, T& mOut)
            {
                auto l(make_unique_lock(mtx));

                // Release the lock and wait until `data` is not empty.
                if(cv.wait_for(l, mDuration, [this]
                       {
                           return !data.empty();
                       }))
                {
                    mOut = dequeue_impl();
                    return true;
                }

                return false;
            }

            auto size() const
            {
                auto l(make_unique_lock(mtx));
                return data.size();
            }

            auto empty() const
            {
                auto l(make_unique_lock(mtx));
                return data.empty();
            }
        };
    }
}
