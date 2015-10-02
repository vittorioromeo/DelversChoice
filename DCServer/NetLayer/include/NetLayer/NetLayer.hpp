#pragma once

// The `Common` module contains:
// * External dependencies.
// * Commonly used utility fucntions and typedefs.
// * API macros.
#include "./Common/Common.hpp"

// The `PacketTypes` module contains:
// * Metaprogramming facilities for packet definitions.
// * Metaprogramming facilities for data serialization
// * User-interface macros.
//include "./PacketTypes/PacketTypes.hpp"

// The `Architecture` module contains:
// * Thread-safe data structures for packet queues.
// * Wrappers around send and receive packet buffers.
// * Managed abstraction for server/client hosts.
#include "./Architecture/Architecture.hpp"