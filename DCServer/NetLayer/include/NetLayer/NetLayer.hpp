#pragma once

// The `Common` module contains:
// * External dependencies.
// * Commonly used utility functions and typedefs.
// * API macros.
#include "./Common/Common.hpp"

// The `Serialization` module contains:
// * Utility functions to serialize/deserialize STL types and NetLayer types.
// * Interface functions to quickly serialize/deserialize from/to objects.
#include "./Serialization/Serialization.hpp"

// The `Payload` module contains:
// * An abstraction consisting of packet data and an address.
// * Utility/interface functions to create payloads.
#include "./Payload/Payload.hpp"

// The `Tunnel` module contains:
// * An abstraction over a data source/sink.
// * A mock implementation for tests.
// * A real implementation for UDP sockets.
#include "./Tunnel/Tunnel.hpp"

// The `Architecture` module contains:
// * Thread-safe data structures for packet queues.
// * Wrappers around send and receive packet buffers.
// * Managed abstraction for server/client hosts.
#include "./Architecture/Architecture.hpp"

// The `Pckt` module contains:
// * A wrapper over `std::tuple` that automatically deals with
// serialization/deserialization.
#include "./Pckt/Pckt.hpp"

// The `Bound` module contains:
// * Metaprogramming facilities for packet definitions.
// * Metaprogramming facilities for data serialization
// * User-interface macros.
#include "./Bound/Bound.hpp"
