#pragma once

#include "./Deps.hpp"

namespace nl
{
using IpAddr = sf::IpAddress;
using PcktBuf = sf::Packet;
using ScktUdp = sf::UdpSocket;
using Port = unsigned short;

using Int = int;
using UInt = unsigned int;
using Sz = ssvu::SizeT;

using Int8 = sf::Int8;
using Int16 = sf::Int16;
using Int32 = sf::Int32;
using Int64 = sf::Int64;

using UInt8 = sf::Uint8;
using UInt16 = sf::Uint16;
using UInt32 = sf::Uint32;
using UInt64 = sf::Uint64;
}

namespace nl
{
// TODO:
template <typename Mutex, typename... Args>
auto mkUniqueLock(Mutex&& m, Args&&... args)
{
    return std::unique_lock<std::remove_reference_t<Mutex>>(
    FWD(m), FWD(args)...);
}
}