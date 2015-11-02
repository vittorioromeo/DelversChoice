#pragma once

#include "../Common/Common.hpp"

// Aggregate of an IP address and a Port.
#include "./PayloadAddress.hpp"

// Combination of a `PAddress` and a `PcktBuf`.
#include "./PayloadImpl.hpp"

// Utility function `make_payload`.
#include "./MkPayload.hpp"

// Stream operators.
#include "./PayloadPrintOps.hpp"

// Send and receive from UDP sockets into/from a `Payload`.
#include "./ScktUtils.hpp"
