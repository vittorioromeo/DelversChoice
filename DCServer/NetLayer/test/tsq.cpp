#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>


int main()
{
    using namespace std;
    using namespace std::literals;

    nl::Impl::ThreadSafeQueue<int> tsq;
    std::vector<std::future<void>> futs;


    int acc = 0;
    int accq = 0;

    std::future<void> proc = std::async(std::launch::async, [&tsq, &accq]
        {
            int i;
            while(tsq.try_dequeue_for(150ms, i))
            {
                accq += i;
            }
        });

    for(int i = 0; i < 100; ++i)
    {
        acc += i;


        futs.emplace_back(std::async(std::launch::async, [&tsq, i]
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(ssvu::getRndI(0, 100)));
                tsq.enqueue(i);
            }));
    }

    for(auto& f : futs) f.get();
    proc.get();

    TEST_ASSERT_OP(acc, ==, accq);

    return 0;
}
