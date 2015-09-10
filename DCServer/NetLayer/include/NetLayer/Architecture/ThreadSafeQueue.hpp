#pragma once

#include <mutex>
#include <condition_variable>
#include "../Common/Common.hpp"

namespace nl
{
	namespace Impl
	{
		template<typename T> class ThreadSafeQueue
		{
			private:
				std::vector<T> data;
				std::mutex mtx;
				std::condition_variable cv;

			public:
				template<typename... TArgs> 
				void enqueue(TArgs&&... mArgs)
				{
					std::unique_lock<std::mutex> l(mtx);
					data.emplace_back(FWD(mArgs)...);

					l.unlock();
					cv.notify_one();
				}

				T dequeue()
				{
					std::unique_lock<std::mutex> l(mtx);
					while(data.empty()) cv.wait(l);

					auto result(data.front());
					data.erase(std::begin(data));
					return result;
				}

				auto empty()
				{
					std::unique_lock<std::mutex> l(mtx);
					return data.empty();
				}
		};
	}
}