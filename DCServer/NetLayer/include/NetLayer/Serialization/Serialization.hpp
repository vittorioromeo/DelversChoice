#pragma once

#include "../Common/Common.hpp"

// Global `operator<<` and `operator>>` overloads to serialize common types.
#include "./SerializationOps.hpp"

// Utility interface functions to aid with ADL resolution and to generate
// `PcktBuf` instances.
#include "./SerializationInterface.hpp"
