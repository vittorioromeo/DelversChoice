#pragma once

#include "../include/NetLayer/NetLayer.hpp"

template <typename T>
auto safe_cin()
{
    T temp;

    std::cin >> temp;
    std::cin.clear();
    std::cin.ignore(2048, '\n');

    return temp;
}

auto ask_input_line(const std::string& x)
{
    std::cout << "Enter " << x << ":\n";

    std::string temp;
    std::getline(std::cin, temp);
    std::cin.clear();

    return temp;
}

template <typename T>
auto ask_input(const std::string& x)
{
    std::cout << "Enter " << x << ":\n";
    return safe_cin<T>();
}

template <typename... Ts>
auto exec_choice(Ts&&... xs)
{
    int next_idx(0);
    constexpr int last_valid_idx(sizeof...(Ts) / 2 - 1);

    ssvu::lo() << "Choose:\n";

    // TODO: vrmc::static_for(...)

    ssvu::forArgs<2>(
        [&next_idx](auto&& t, auto&&)
        {
            if(next_idx != last_valid_idx)
            {
                ssvu::lo() << next_idx;
            }
            else
            {
                ssvu::lo() << "_";
            }

            ssvu::lo() << ") " << t << "\n";

            ++next_idx;
        },
        FWD(xs)...);

    auto choice(safe_cin<int>());
    next_idx = 0;
    ssvu::forArgs<2>(
        [&next_idx, &choice](auto&& t, auto&& f)
        {
            if((next_idx == last_valid_idx && choice >= last_valid_idx) ||
                choice == next_idx)
            {
                ssvu::lo() << "Choice: " << t << "\n";
                f();
            }

            ++next_idx;
        },
        FWD(xs)...);
}
