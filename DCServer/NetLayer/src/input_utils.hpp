#pragma once

#include "../include/NetLayer/NetLayer.hpp"

template <typename T>
auto getInput(const std::string& title)
{
    T input;
    std::cin >> input;

    ssvu::lo(title) << input << "\n";
    return input;
}

auto getInputLine(const std::string& title)
{
    std::string input;
    std::getline(std::cin, input);

    // ssvu::lo(title) << input << "\n";
    return input;
}
