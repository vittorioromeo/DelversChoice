#pragma once

// The `Common` module contains:
// * External dependencies.
// * Commonly used utility functions and typedefs.
// * API macros.
#include "./Common/Common.hpp"

// The `PacketTypes` module contains:
// * Metaprogramming facilities for packet definitions.
// * Metaprogramming facilities for data serialization
// * User-interface macros.
// include "./PacketTypes/PacketTypes.hpp"

// TODO:
#include "./Serialization/Serialization.hpp"

// TODO:
#include "./Payload/Payload.hpp"

// TODO:
#include "./Tunnel/Tunnel.hpp"

// The `Architecture` module contains:
// * Thread-safe data structures for packet queues.
// * Wrappers around send and receive packet buffers.
// * Managed abstraction for server/client hosts.
#include "./Architecture/Architecture.hpp"

// TODO:
#include "./Pckt/Pckt.hpp"



// TODO:
#include "./Bound/Bound.hpp"



// TODO:
// Thesis:
// * TDD development
// * Layered architecture
// * Doxygen + doxygraph for UML
// * Use AutoSyncGen as thesis template
// * Dependencies
// * AutoSyncGen integration possibility?
// * Why C++, why C++14
// * User freedom
