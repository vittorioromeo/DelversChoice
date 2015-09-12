#pragma once

#include <mutex>
#include <condition_variable>
#include "../Common/Common.hpp"

namespace nl
{
	namespace Impl
	{
		template<typename T> 
		class ThreadSafeQueue
		{
			private:
				std::deque<T> data;
				std::mutex mtx;
				std::condition_variable cv;

			public:
				~ThreadSafeQueue()
				{
					NL_DEBUGLO() << "~queue";
				}

				template<typename... TArgs> 
				void enqueue(TArgs&&... mArgs)
				{	
					{
						std::unique_lock<std::mutex> l(mtx);
						data.emplace_front(FWD(mArgs)...);
					}
					
					cv.notify_one();
				}

				T dequeue()
				{
					std::unique_lock<std::mutex> l(mtx);
					cv.wait(l, [this]{ return !data.empty(); });

					auto result(data.back());
					data.pop_back();
					return result;
				}

				auto size()
				{
					std::unique_lock<std::mutex> l(mtx);
					return data.size();
				}

				auto empty()
				{
					std::unique_lock<std::mutex> l(mtx);
					return data.empty();
				}
		};
	}
}