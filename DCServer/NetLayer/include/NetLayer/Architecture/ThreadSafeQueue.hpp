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
            mutable std::mutex mtx;
            std::condition_variable cv;

        public:
            // TODO: remove
            ~ThreadSafeQueue() { NL_DEBUGLO() << "~queue"; }

            template <typename... TArgs>
            void enqueue(TArgs&&... mArgs)
            {
                {
                    std::unique_lock<std::mutex> l(mtx);
                    data.emplace_front(FWD(mArgs)...);
                }

                // Notify one thread waiting to `dequeue`.
                cv.notify_one();
            }

            T dequeue()
            {
                std::unique_lock<std::mutex> l(mtx);

                // Release the lock and wait until `data` is not empty.
                cv.wait(l, [this]
                        {
                            return !data.empty();
                        });

                auto result(data.back());
                data.pop_back();
                return result;
            }

            auto size() const
            {
                std::unique_lock<std::mutex> l(mtx);
                return data.size();
            }

            auto empty() const
            {
                std::unique_lock<std::mutex> l(mtx);
                return data.empty();
            }
        };
    }
}
