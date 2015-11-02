#pragma once

// Lock-based thread safe queue.
#include "../Architecture/ThreadSafeQueue.hpp"

// Abstraction over a packet buffer (queue).
#include "../Architecture/ManagedPcktBuf.hpp"

// Outgoing packet queue + buffer.
#include "../Architecture/ManagedSendBuf.hpp"

// Incoming packet queue + buffer.
#include "../Architecture/ManagedRecvBuf.hpp"

// Managed host.
// * Has a send buffer.
// * Has a receive buffer.
// * Has a dedicated thread to receive data from the internet.
// * Has a dedicated thread to send data to the internet.
#include "../Architecture/ManagedHost.hpp"
